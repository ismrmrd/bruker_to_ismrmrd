// TestConversion.cpp
// simplest cpp version converting Bruker PVM to HDF5 for ISMRMRD
//
// MJL and SJI 3/25/15
//

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>

#include "brukerrawdata.hpp"
#include "brukerparameterparser.hpp"

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/version.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{

    std::string in_filename;
    std::string out_filename;
    std::string out_group;
    
    // Set up the command line interface options
    po::options_description desc("Allowed options");

    desc.add_options()
        ("help,h", "produce help message")
        ("filename,f", po::value<std::string>(&in_filename), "Input file")
        ("outfile,o", po::value<std::string>(&out_filename)->default_value("out.h5"), "Output file")
        ("out-group,G", po::value<std::string>(&out_group)->default_value("dataset"), "Output group name")  
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (!vm.count("filename")) {
        std::cout << std::endl << std::endl << "\tYou must supply a filename" << std::endl << std::endl;
        std::cout << desc << std::endl;
        return -1;
    }

    std::cout << "Burker ISMRMRD converter" << std::endl;

    // The names of the files in the Bruker dataset
    std::string fidfilename = in_filename + std::string("/fid");
    std::string acqpfilename = in_filename + std::string("/acqp");
    std::string methodfilename = in_filename + std::string("/method");

    // Parse Bruker parameters
    BrukerParameterFile acqpar(acqpfilename);
    BrukerParameterFile methodpar(methodfilename);

    // Get the profile list
    BrukerProfileListGenerator lg;
    BrukerRawDataProfile* first = lg.GetProfileList(&acqpar,&methodpar);

    int size_kx = lg.GetDimensionSize(0);
    int size_ky = lg.GetDimensionSize(1); if (size_ky == 0) size_ky++;
    int size_kz = lg.GetDimensionSize(2); if (size_kz == 0) size_kz++;
    int no_objects = lg.GetNumberOfObjects();
    int no_repetitions = lg.GetNumberOfRepetitions();

    int ky_min = lg.GetMinEncodingStep1();
    int ky_max = lg.GetMaxEncodingStep1();
    int kz_min = lg.GetMinEncodingStep2();
    int kz_max = lg.GetMaxEncodingStep2();

    BrukerParameter* p;
    p = acqpar.FindParameter("SW");
    int freq = floor(p->GetValue(0)->GetFloatValue() * 1000000);

    p = methodpar.FindParameter("PVM_EncAvailReceivers");
    int nc = p->GetValue(0)->GetIntValue();
    
    p = methodpar.FindParameter("PVM_EncMatrix");
    int nx = p->GetValue(0)->GetIntValue();
    int ny = p->GetValue(1)->GetIntValue();
    int nz = lg.GetNumberOfObjects();
 
    p = methodpar.FindParameter("PVM_Fov");
    int fovx = p->GetValue(0)->GetIntValue();
    int fovy = p->GetValue(1)->GetIntValue();
    p = acqpar.FindParameter("ACQ_slice_thick");
    int fovz = p->GetValue(0)->GetIntValue();
        
    // Write some info out to the user
    //lg.PrintParameters();            
    //std::cout << "Frequency: " << freq << std::endl;
    //std::cout << "Number of channels: " << nc << std::endl;
    //std::cout << "Nx: " << nx << std::endl;
    //std::cout << "Ny: " << ny << std::endl;
    //std::cout << "Nz: " << nz << std::endl;
    //std::cout << "FOV_x: " << fovx << std::endl;
    //std::cout << "FOV_y: " << fovy << std::endl;
    //std::cout << "FOV_z: " << fovz << std::endl;
    
    // Some initializations
    ISMRMRD::Acquisition acq;
    
    // Create the dataset
    ISMRMRD::Dataset dataset(out_filename.c_str(), out_group.c_str());

    // open input fid file
    std::ifstream file_1(fidfilename.c_str());
    if (!file_1)
    {
        std::cout << "Provided Bruker file cannot be open or does not exist." << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Bruker file is: " << in_filename << std::endl;
    }

    //Let's create a header, we will use the C++ classes in ismrmrd/xml.h
    ISMRMRD::IsmrmrdHeader h;
    h.version = ISMRMRD_XMLHDR_VERSION;
    h.experimentalConditions.H1resonanceFrequency_Hz = freq;

    ISMRMRD::AcquisitionSystemInformation sys;
    sys.institutionName = "Mouse Imaging Facility";
    sys.receiverChannels = nc;
    h.acquisitionSystemInformation = sys;

    //Create an encoding section
    ISMRMRD::Encoding e;
    e.encodedSpace.matrixSize.x = nx;
    e.encodedSpace.matrixSize.y = ny;
    e.encodedSpace.matrixSize.z = 1;
    e.encodedSpace.fieldOfView_mm.x = fovx;
    e.encodedSpace.fieldOfView_mm.y = fovy;
    e.encodedSpace.fieldOfView_mm.z = fovz;
    e.reconSpace.matrixSize.x = nx;
    e.reconSpace.matrixSize.y = ny;
    e.reconSpace.matrixSize.z = 1;
    e.reconSpace.fieldOfView_mm.x = fovx;
    e.reconSpace.fieldOfView_mm.y = fovy;
    e.reconSpace.fieldOfView_mm.z = fovz;
    e.trajectory = "cartesian";
    e.encodingLimits.kspace_encoding_step_1 = ISMRMRD::Limit(0, ny-1, ny/2);
    e.encodingLimits.slice = ISMRMRD::Limit(0, nz-1, nz/2);

    //Add the encoding section to the header
    h.encoding.push_back(e);

    //Add any additional fields that you may want would go here....

    //Serialize the header
    std::stringstream str;
    ISMRMRD::serialize( h, str);
    std::string xml_header = str.str();
    //std::cout << xml_header << std::endl;
        
    //Write the header to the data file.
    dataset.writeHeader(xml_header);
    std::cout << "Wrote XML header" << std::endl;

    // Create an acquisition structure
    acq.resize(nx,nc);
    acq.center_sample() = nx/2;

    // These are definitely wrong
    // Consider looking at ACQ_grad_matrix?
    acq.read_dir()[0] = 1.;
    acq.phase_dir()[1] = 1.;
    acq.slice_dir()[2] = 1.;

    // loop over data set to read it in, convert it and write it out
    // a buffer for reading the data
    int32_t kdata[2*nx*nc];

    int counter = 0;
    for(int i=0;i<ny;i++) {
        for(int j=0;j<nz;j++) {
            acq.scan_counter() = counter;
            acq.idx().kspace_encode_step_1 = i;
            acq.idx().slice = j;
           
            //Set some flags
            acq.clearAllFlags();
            if (i == 0) {
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_SLICE);
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1);
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_REPETITION);
            }
            if (i == (ny-1)) {
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE);
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1);
                acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_REPETITION);
            }

            file_1.read(reinterpret_cast<char*>(kdata), sizeof(int32_t) * (nx*nc*2));
            // need to convert to complex float
            for (int c=0; c<nc; c++) {
                for (int s=0; s< nx; s++) {
                    acq.data(s,c) = std::complex<float>(kdata[2*(c*nx+s)], kdata[2*(c*nx+s)+1]);
                }
            }
            dataset.appendAcquisition(acq);
            counter++;
        }
    }

    // Close the Bruker file (is this necessary?)
    file_1.close();

    // Goodbye
    std::cout << "Conversion complete." << std::endl;

}

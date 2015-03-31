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
            ("no-subject,N","no subject information") 
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

    bool subject_info = true;
    if(vm.count("no-subject")) subject_info = false;

    std::cout << "Bruker ISMRMRD converter" << std::endl;

    // The names of the files in the Bruker dataset
    std::string fidfilename = in_filename + std::string("/fid");
    std::string acqpfilename = in_filename + std::string("/acqp");
    std::string methodfilename = in_filename + std::string("/method");
    unsigned slashloc = in_filename.find_last_of("/");
    std::string subjectfilename = in_filename.substr(0,slashloc+1) + std::string("subject");

    // Parse Bruker parameters
    BrukerParameterFile acqpar(acqpfilename);
    BrukerParameterFile methodpar(methodfilename);
    BrukerParameterFile subjectpar(subjectfilename);

    // Get the profile list and the first profile
    BrukerProfileListGenerator lg;
    BrukerRawDataProfile* first = lg.GetProfileList(&acqpar,&methodpar);

    // Some parameters from the profile list
    int size_kx = lg.GetDimensionSize(0);
    int size_ky = lg.GetDimensionSize(1); if (size_ky == 0) size_ky++;
    int size_kz = lg.GetDimensionSize(2); if (size_kz == 0) size_kz++;
    int no_objects = lg.GetNumberOfObjects();
    int no_echos = lg.GetNumberOfEchos();
    int no_repetitions = lg.GetNumberOfRepetitions();
    int ky_min = lg.GetMinEncodingStep1();
    int ky_max = lg.GetMaxEncodingStep1();
    int kz_min = lg.GetMinEncodingStep2();
    int kz_max = lg.GetMaxEncodingStep2();

    // Some parameters from the acqp and method files
    // TODO most of these can probably found in the lg above
    // have to add some methods to the BrukerProfileListGenerator
    BrukerParameter* p;
    p = acqpar.FindParameter("SW");
    int freq = floor(p->GetValue(0)->GetFloatValue() * 1000000);

    // Encoding info
    //p = methodpar.FindParameter("PVM_SpatDimEnum");
    //std::string image_type = p->GetValue(0)->GetStringValue(); // 2D or 3D acq marker
    //bool threed = image_type.compare("2D");
    
    p = methodpar.FindParameter("PVM_EncMatrix");
    int nx = p->GetValue(0)->GetIntValue();
    int ny = p->GetValue(1)->GetIntValue();
    int nz = lg.GetNumberOfObjects() / lg.GetNumberOfEchos();
    int nc = lg.GetNumberOfChannels();

    p = methodpar.FindParameter("PVM_Fov");
    int fovx = p->GetValue(0)->GetIntValue();
    int fovy = p->GetValue(1)->GetIntValue();
    p = acqpar.FindParameter("ACQ_slice_thick");
    int fovz = p->GetValue(0)->GetIntValue();

    // Subject Info
    p = subjectpar.FindParameter("SUBJECT_name_string");
    std::string subject_name = p->GetValue(0)->GetStringValue();
    p = subjectpar.FindParameter("SUBJECT_study_name");
    std::string study_description = p->GetValue(0)->GetStringValue();
    p = subjectpar.FindParameter("SUBJECT_study_instance_uid");
    std::string study_instanceUID = p->GetValue(0)->GetStringValue();
    p = subjectpar.FindParameter("SUBJECT_entry");
    std::string patient_entry = p->GetValue(0)->GetStringValue();
    p = subjectpar.FindParameter("SUBJECT_position");
    std::string patient_position = p->GetValue(0)->GetStringValue();
    p = subjectpar.FindParameter("SUBJECT_date");
    std::string study_date = p->GetValue(0)->GetStringValue();

    // Geometry
    float read_offset_mm[nz];
    p = acqpar.FindParameter("ACQ_read_offset");
    for ( int i=0; i< nz; i++ ) {
        read_offset_mm[i] = p->GetValue(i)->GetFloatValue();
    }
    
    float phase1_offset_mm[nz];
    p = acqpar.FindParameter("ACQ_phase1_offset");
    for ( int i=0; i<nz; i++ ) {
        phase1_offset_mm[i] = p->GetValue(i)->GetFloatValue();
    }
    float phase2_offset_mm[nz];
    p = acqpar.FindParameter("ACQ_phase2_offset");
    for ( int i=0; i<nz; i++ ) {
        phase2_offset_mm[i] = p->GetValue(i)->GetFloatValue();
    }
    float slice_offset_mm[nz];
    p = acqpar.FindParameter("ACQ_slice_offset");
    for ( int i=0; i<nz; i++ ) {
        slice_offset_mm[i] = p->GetValue(i)->GetFloatValue();
    }
    int obj_order[nz]; // slice order
    p = acqpar.FindParameter("ACQ_obj_order");
    for ( int i=0; i < nz; i++ ) {
        obj_order[i] = p->GetValue(i)->GetIntValue();
    }
    float grad_matrix[nz][3][3]; // orientation info
    p = acqpar.FindParameter("ACQ_grad_matrix");
    int gmcnt=0;
    for ( int i=0; i<nz; i++ ) {
        for ( int j=0; j<3; j++ ) {
            for ( int k=0; k<3; k++ ) {
                 grad_matrix[i][j][k] = p->GetValue(gmcnt)->GetFloatValue();
                 gmcnt++;
            }
         }
     }

    std::cout << "YO MAMA!" << std::endl;
    

    // Write some info out to the user
    //lg.PrintParameters();            
    //std::cout << "Spatial Dimensions: " << image_type << std::endl;
    std::cout << "Subject name: " << subject_name << std::endl;
    std::cout << "Study description: " << study_description << std::endl;
    std::cout << "Study instance: " << study_instanceUID << std::endl;
    std::cout << "Patient entry: " << patient_entry << std::endl;
    std::cout << "Patient Position: " << patient_position << std::endl;    
    //std::cout << "Frequency: " << freq << std::endl;
    //std::cout << "Number of channels: " << nc << std::endl;
    //std::cout << "Nx: " << nx << std::endl;
    //std::cout << "Ny: " << ny << std::endl;
    //std::cout << "Nz: " << nz << std::endl;
    //std::cout << "FOV_x: " << fovx << std::endl;
    //std::cout << "FOV_y: " << fovy << std::endl;
    //std::cout << "FOV_z: " << fovz << std::endl;

    // Create the dataset
    ISMRMRD::Dataset dataset(out_filename.c_str(), out_group.c_str());

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
    e.encodedSpace.matrixSize.z = size_kz;
    e.encodedSpace.fieldOfView_mm.x = fovx;
    e.encodedSpace.fieldOfView_mm.y = fovy;
    e.encodedSpace.fieldOfView_mm.z = fovz;
    e.reconSpace.matrixSize.x = nx;
    e.reconSpace.matrixSize.y = ny;
    e.reconSpace.matrixSize.z = size_kz;
    e.reconSpace.fieldOfView_mm.x = fovx;
    e.reconSpace.fieldOfView_mm.y = fovy;
    e.reconSpace.fieldOfView_mm.z = fovz;
    e.trajectory = "cartesian";
    e.encodingLimits.kspace_encoding_step_1 = ISMRMRD::Limit(0, ny-1, ny/2);
    if (size_kz > 1) {
        e.encodingLimits.kspace_encoding_step_2 = ISMRMRD::Limit(0, size_kz-1, size_kz/2);
    }
    if (nz > 1) {
        e.encodingLimits.slice = ISMRMRD::Limit(0, nz-1, nz/2);
    }
    if (no_echos > 1) {
        e.encodingLimits.contrast = ISMRMRD::Limit(0, no_echos-1, 0);
    }

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

    // Create an ISMRMRD acquisition
    ISMRMRD::Acquisition acq;
    acq.resize(nx,nc);
    acq.center_sample() = nx/2;

    // TODO: These are definitely wrong
    // Consider looking at ACQ_grad_matrix?
    acq.read_dir()[0] = 1.;
    acq.phase_dir()[1] = 1.;
    acq.slice_dir()[2] = 1.;

    // open input fid file
    std::ifstream fidfile;
    fidfile.open(fidfilename.c_str(),std::ifstream::in | std::ifstream::binary );
    if (!fidfile)
    {
        std::cerr << "Error opening fid file" << in_filename << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Reading from fid file " << in_filename << std::endl;
    }

    // Loop over data set to read it in, convert it and write it out
    int64_t counter = 0;
    BrukerRawDataProfile* current = first;
    float* data_ptr = 0;

    while (current) {

        acq.scan_counter() = counter;
        acq.idx().kspace_encode_step_1 = current->GetEncodeStep1()+(size_ky>>1);
        acq.idx().kspace_encode_step_2 = current->GetEncodeStep2()+(size_kz>>1);
        acq.idx().slice = current->GetSliceNo();
        acq.idx().contrast = current->GetEchoNo();
        acq.idx().repetition = current->GetRepetitionNo();
        int curslice = counter % nz;
        for ( int i=0; i<2; i++ ) {
             acq.read_dir()[i] = grad_matrix[curslice][0][i];
             acq.phase_dir()[i] = grad_matrix[curslice][1][i];
             acq.slice_dir()[i] = grad_matrix[curslice][2][i];
        }

        // Set some flags
        // TODO: this needs fleshing out
        acq.clearAllFlags();
        if (current->GetEncodeStep1() == ky_min) {
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_SLICE);
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1);
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_FIRST_IN_REPETITION);
        }
        if (current->GetEncodeStep1() == ky_max) {
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE);
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1);
            acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_REPETITION);
        }

        // read the data
        // convert to complex float and stuff
        current->SetProfileLength(nx*nc);
        current->ReadData(fidfile);
        data_ptr = current->GetDataPtr();
        for (int c=0; c<nc; c++) {
            for (int s=0; s< nx; s++) {
                acq.data(s,c) = std::complex<float>(data_ptr[2*(nx*c+s)], data_ptr[2*(nx*c+s)+1]);
            }
        }

        // append to the dataset
        dataset.appendAcquisition(acq);

        // next
        current = current->GetNext();
        counter++;
    }

    // Close the Bruker file (is this necessary?)
    fidfile.close();

    // Goodbye
    std::cout << "Conversion complete." << std::endl;

}

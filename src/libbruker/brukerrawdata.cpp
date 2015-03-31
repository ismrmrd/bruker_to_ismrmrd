#include "brukerrawdata.hpp"
#include <iostream>

BrukerRawDataProfile::BrukerRawDataProfile()
  : m_uiProfileLength(0),
    m_uiNumberOfChannels(0),
    m_iEncodeStep1(0),
    m_iEncodeStep2(0),
    m_uiChannelNo(0),
    m_uiObjectNo(0),
    m_uiSliceNo(0),
    m_uiEchoNo(0),
    m_uiRepetitionNo(0),
    m_ulFilePosition(0),
    m_pData(0),
    m_pNext(0),
    m_pPrevious(0),
    m_DataFormat(BrukerRawDataProfile::GO_FORMAT_NONE)
{

}

BrukerRawDataProfile::~BrukerRawDataProfile()
{
  DeAllocateMemory();
}

void BrukerRawDataProfile::SetProfileLength(unsigned int length)
{
  m_uiProfileLength = length;
}

unsigned int BrukerRawDataProfile::GetProfileLength()
{

  return m_uiProfileLength;
}

void BrukerRawDataProfile::SetNumberOfChannels(unsigned int nchan)
{
  m_uiNumberOfChannels = nchan;
}

unsigned int BrukerRawDataProfile::GetNumberOfChannels()
{

  return m_uiNumberOfChannels;
}

void BrukerRawDataProfile::SetEncodeStep1(int e)
{
  m_iEncodeStep1 = e;
}

int BrukerRawDataProfile::GetEncodeStep1()
{
  return m_iEncodeStep1;
}

void BrukerRawDataProfile::SetEncodeStep2(int e)
{
  m_iEncodeStep2 = e;
}

int BrukerRawDataProfile::GetEncodeStep2()
{
  return m_iEncodeStep2;
}

void BrukerRawDataProfile::SetChannelNo(unsigned int c)
{
  m_uiChannelNo = c;
}

unsigned int BrukerRawDataProfile::GetChannelNo()
{
  return m_uiChannelNo;
}

void BrukerRawDataProfile::SetObjectNo(unsigned int o)
{
  m_uiObjectNo = o;
}

unsigned int BrukerRawDataProfile::GetObjectNo()
{
  return m_uiObjectNo;
}

void BrukerRawDataProfile::SetSliceNo(unsigned int s)
{
  m_uiSliceNo = s;
}

unsigned int BrukerRawDataProfile::GetSliceNo()
{
  return m_uiSliceNo;
}

void BrukerRawDataProfile::SetEchoNo(unsigned int e)
{
  m_uiEchoNo = e;
}

unsigned int BrukerRawDataProfile::GetEchoNo()
{
  return m_uiEchoNo;
}

void BrukerRawDataProfile::SetRepetitionNo(unsigned int r)
{
  m_uiRepetitionNo = r;
}

unsigned int BrukerRawDataProfile::GetRepetitionNo()
{
  return m_uiRepetitionNo;
}

void BrukerRawDataProfile::SetFilePosition(unsigned long int p)
{
  m_ulFilePosition = p;
}

unsigned long int BrukerRawDataProfile::GetFilePosition()
{
  return m_ulFilePosition;
}

void BrukerRawDataProfile::SetNext(BrukerRawDataProfile* p)
{
  m_pNext = p;
}

BrukerRawDataProfile* BrukerRawDataProfile::GetNext()
{
  return m_pNext;
}


void BrukerRawDataProfile::SetPrevious(BrukerRawDataProfile* p)
{
  m_pPrevious = p;
}


BrukerRawDataProfile* BrukerRawDataProfile::GetPrevious()
{
  return m_pPrevious;
}

void BrukerRawDataProfile::SetDataFormat(BrukerDataFormat f)
{
  m_DataFormat = f;
}

BrukerRawDataProfile::BrukerDataFormat BrukerRawDataProfile::GetDataFormat()
{
  return m_DataFormat;
}

void BrukerRawDataProfile::ReadData(std::ifstream& fs)
{
  if (m_DataFormat == GO_FORMAT_NONE) {
    return;
  }

  DeAllocateMemory();
  m_pData = new float[m_uiProfileLength*2];
  if (!m_pData) {
    std::cerr << "BrukerRawDataProfile: memory allocation failed!" << std::endl;
    return;
  }

  if (static_cast<unsigned long>(fs.tellg()) != m_ulFilePosition) {
    fs.seekg(static_cast<std::streampos>(m_ulFilePosition), std::ios::beg);
  }
  
  short* ShortBuffer = 0;
  int* IntBuffer = 0;

  switch (m_DataFormat) {

  case GO_16BIT_SGN_INT:
    ShortBuffer = new short[m_uiProfileLength*2];
    if (!ShortBuffer) {
      std::cerr << "BrukerRawDataProfile: Unable to allocate short buffer" << std::endl;
      return;
    }
    fs.read((char*)ShortBuffer, m_uiProfileLength*2*sizeof(short));
    if (static_cast<unsigned int>(fs.gcount()) != m_uiProfileLength*2*sizeof(short)) {
      std::cerr << "BrukerRawDataProfile: Unable to read sufficient bytes from stream" << std::endl;
      return;
    }
    for (unsigned int i = 0; i < m_uiProfileLength; i++) {
      m_pData[i*2  ] = ShortBuffer[i*2  ]; /* Real */
      m_pData[i*2+1] = ShortBuffer[i*2+1]; /* Imag */

    }
    delete [] ShortBuffer;
    ShortBuffer = 0;
    break;

  case GO_32BIT_SGN_INT:
    IntBuffer = new int[m_uiProfileLength*2];
    if (!IntBuffer) {
      std::cerr << "BrukerRawDataProfile: Unable to allocate int buffer" << std::endl;
      return;
    }
    fs.read((char*)IntBuffer, m_uiProfileLength*2*sizeof(int));
    if (static_cast<unsigned int>(fs.gcount()) != m_uiProfileLength*2*sizeof(int)) {
      std::cerr << "BrukerRawDataProfile: Unable to read sufficient bytes from stream" << std::endl;
      return;
    }
    for (unsigned int i = 0; i < m_uiProfileLength; i++) {
      m_pData[i*2  ] = IntBuffer[i*2  ]; /* Real */
      m_pData[i*2+1] = IntBuffer[i*2+1]; /* Imag */
      //std::cout << "(" << m_pData[i*2  ] << ", " << m_pData[i*2+1] << ")" << std::endl;
    }
    delete [] IntBuffer;
    IntBuffer = 0;
    break;

  case GO_32BIT_FLOAT:
    fs.read((char*)m_pData, m_uiProfileLength*2*sizeof(float));
    if (static_cast<unsigned int>(fs.gcount()) != m_uiProfileLength*2*sizeof(float)) {
      std::cerr << "BrukerRawDataProfile: Unable to read sufficient bytes from stream" << std::endl;
      return;
    }
    break;

  default:
    std::cerr << "BrukerRawDataProfile: Unknow data type in read" << std::endl;
    return;
  }

}

void BrukerRawDataProfile::WriteData(std::ofstream& fs, float max_val)
{


  if (m_DataFormat == GO_FORMAT_NONE) {
    std::cerr << "BrukerRawDataProfile::WriteData: Data Format not set, unable to write" << std::endl;
    return;
  }

  if (!m_pData) {
    std::cerr << "BrukerRawDataProfile::WriteData: Raw data not allocated, unable to write" << std::endl;
    return;
  }


  if (static_cast<unsigned long>(fs.tellp()) != m_ulFilePosition) {
    fs.seekp(static_cast<std::streampos>(m_ulFilePosition), std::ios::beg);
  }
  
  short* ShortBuffer = 0;
  int* IntBuffer = 0;
  float scale = 0.0;

  switch (m_DataFormat) {

  case GO_16BIT_SGN_INT:
    try {
      ShortBuffer = new short[m_uiProfileLength*2];
    } catch (...) {
      std::cerr << "BrukerRawDataProfile: Unable to allocate short buffer" << std::endl;
      return;
    }
    
    //Convert and scale data
    scale = (max_val != 0.0 ? SHRT_MAX / (max_val * 1.1) : 0.0);
    for (unsigned int i = 0; i < m_uiProfileLength*2; i++) {
      ShortBuffer[i] = static_cast<short>(scale*m_pData[i]+0.5);
    }

    try {
      fs.write((char*)ShortBuffer, m_uiProfileLength*2*sizeof(short));
    } catch(...) {
      std::cerr << "BrukerRawDataProfile: Unable to write sufficient bytes to stream" << std::endl;
      return;
    }

    delete [] ShortBuffer;
    ShortBuffer = 0;
    break;


  case GO_32BIT_SGN_INT:

    try {
      IntBuffer = new int[m_uiProfileLength*2];
    } catch (...) {
      std::cerr << "BrukerRawDataProfile: Unable to allocate short buffer" << std::endl;
      return;
    }
    
    //Convert and scale data
    scale = (max_val != 0.0 ? INT_MAX / (max_val * 1.1) : 0.0);
    for (unsigned int i = 0; i < m_uiProfileLength*2; i++) {
      IntBuffer[i] = static_cast<int>(scale*m_pData[i]+0.5);
    }

    try {
      fs.write((char*)IntBuffer, m_uiProfileLength*2*sizeof(int));
    } catch(...) {
      std::cerr << "BrukerRawDataProfile: Unable to write sufficient bytes to stream" << std::endl;
      return;
    }

    delete [] IntBuffer;
    ShortBuffer = 0;
    break;

  case GO_32BIT_FLOAT:
    try { 
      fs.write((char*)m_pData, m_uiProfileLength*2*sizeof(float));
    } catch (...) {
      std::cerr << "BrukerRawDataProfile: Unable to write sufficient bytes to stream" << std::endl;
      return;
    }
    break;

  default:
    std::cerr << "BrukerRawDataProfile: Unknow data type in read" << std::endl;
    return;
  }
}



void BrukerRawDataProfile::SetRawData(float* d)
{
  if (!d) return;

  if (!m_pData) {
    try {
      m_pData = new float[m_uiProfileLength*2];
    } catch (...) {
      std::cerr << "BrukerRawdataProfile: Unable to allocate memory for SetRawData" << std::endl;
      return;
    }
  }

  for (unsigned int i = 0; i < m_uiProfileLength*2; i++) m_pData[i] = d[i];
}


void BrukerRawDataProfile::SetRawDataFromArray(mr_recon::ComplexFloatArray& a, int ky_min, int kz_min)
{
  //This function expects the array to have dimensions [kx, ky, kz, objects, repetitions]
  if (ky_min <= - 10000) {
    ky_min = -(a.get_size(1)>>1);
  }

  if (kz_min <= - 10000) {
    kz_min = -(a.get_size(2)>>1);
  }

  //First let's check the dimensions of the array
  if (static_cast<unsigned int>(a.get_size(0)) != m_uiProfileLength) {
    std::cerr << "BrukerRawDataProfile::SetRawDataFromArray: Mismatch between array dimensions(0) = " 
	      << a.get_size(0) << " and uiProfileLength " << m_uiProfileLength << std::endl;
    return;
  }


  if (a.get_size(1) <= (m_iEncodeStep1-ky_min) || ((m_iEncodeStep1-ky_min) < 0)) {
    std::cerr << "BrukerRawDataProfile::SetRawDataFromArray: Mismatch between array dimensions(1) =  " 
	      << a.get_size(1) << " and iEncodeStep1 " << m_iEncodeStep1 << std::endl;
    std::cerr << "a.get_size(1) = " << a.get_size(1) << std::endl;
    std::cerr << "m_iEncodeStep1 = " << m_iEncodeStep1 << std::endl;
    std::cerr << "ky_min = " << ky_min << std::endl;
    std::cerr << "(m_iEncodeStep1-ky_min) = " << (m_iEncodeStep1-ky_min) << std::endl;
    return;
  }


  if (a.get_size(2) <= static_cast<int>((m_iEncodeStep2-kz_min)) || ((m_iEncodeStep2-kz_min) < 0)) {
    std::cerr << "BrukerRawDataProfile::SetRawDataFromArray: Mismatch between array dimensions(2) =  " 
	      << a.get_size(2) << " and iEncodeStep2 " << m_iEncodeStep2 << std::endl;
    return;
  }

  if (static_cast<unsigned int>(a.get_size(3)) <= m_uiObjectNo) {
    std::cerr << "BrukerRawDataProfile::SetRawDataFromArray: Mismatch between array dimensions(3) =  " 
	      << a.get_size(3) << " and uiObjectNo " << m_uiObjectNo << std::endl;
    return;
  }

  if (static_cast<unsigned int>(a.get_size(4)) <=  m_uiRepetitionNo) {
    std::cerr << "BrukerRawDataProfile::SetRawDataFromArray: Mismatch between array dimensions(4) =  " 
	      << a.get_size(4) << " and uiRepetitionNo " << m_uiRepetitionNo << std::endl;
    return;
  }

  unsigned long int index = 
    m_uiRepetitionNo*(a.get_size(3)*a.get_size(2)*a.get_size(1)*a.get_size(0)) +
    m_uiObjectNo*(a.get_size(2)*a.get_size(1)*a.get_size(0)) +
    (m_iEncodeStep2-kz_min)*(a.get_size(1)*a.get_size(0)) +
    (m_iEncodeStep1-ky_min)*(a.get_size(0));

  float* data_p = (float*)(a.get_data_ptr());
  data_p += (index*2);

  this->SetRawData(data_p);
}

void BrukerRawDataProfile::DeAllocateMemory()
{
  if (m_pData) {
    delete [] m_pData;
    m_pData = 0;
  }
}

void BrukerRawDataProfile::DeleteNext()
{
  BrukerRawDataProfile* next = GetNext();
  if (next) {
    m_pNext = next->GetNext();
    if (m_pNext) {
      m_pNext->SetPrevious(this);
    }
    delete next;
  }
}

void BrukerRawDataProfile::DeletePrevious()
{
  BrukerRawDataProfile* prev = GetPrevious();
  if (prev) {
    m_pPrevious = prev->GetPrevious();
    if (m_pPrevious) {
      m_pPrevious->SetNext(this);
    }       
    delete prev;
  }
}

void BrukerRawDataProfile::DeleteLinkedProfiles()
{
  while (GetPrevious()) DeletePrevious();
  while (GetNext()) DeleteNext();
  
}

BrukerRawDataProfile* BrukerRawDataProfile::SortLinkedProfilesByFilePosition()
{
  BrukerRawDataProfile *current, *next, *tmpprev, *tmpnext;

  //Let's find the first profile
  current = this;
  while (current->GetPrevious()) current = current->GetPrevious();

  while (1) {
    int AnyChange;
    AnyChange = 0;
    while(1) {
      next = current->GetNext();
      if (next==NULL) break; // reached end
      if (current->GetFilePosition() > next->GetFilePosition()) { // Swap -> current becomes next
	tmpprev = current->GetPrevious();
	tmpnext = next->GetNext();
	next->SetPrevious(tmpprev);
	next->SetNext(current);
	current->SetPrevious(next);
	current->SetNext(tmpnext);
	if (tmpprev!=NULL) tmpprev->SetNext(next);
	if (tmpnext!=NULL) tmpnext->SetPrevious(current);
	AnyChange = 1;
      } else {  // move to next
	current = next;
      }
    }
    if (AnyChange==0) break; // Finished sort
  }

  //Find the top of the list
  while (current->GetPrevious()) current = current->GetPrevious();

  return current;

}

float BrukerRawDataProfile::GetMaxDataValue(bool include_all_linked_profiles)
{
  float max_val = 0.0;
  if (include_all_linked_profiles) {
    float local_max_val = 0.0;
    BrukerRawDataProfile* current = this;
    while (current->GetPrevious()) current = current->GetPrevious();
    while (current) {
      local_max_val = current->GetMaxDataValue(false);
      if (local_max_val > max_val) max_val = local_max_val;
      current = current->GetNext();
    }
  } else {
    if (m_pData) {
      for (unsigned int i = 0; i < m_uiProfileLength*2; i++) {
	if (fabs(m_pData[i]) > max_val) max_val = m_pData[i];
      }
    }
  }
  return max_val;
}


BrukerProfileListGenerator::BrukerProfileListGenerator()
  : m_ACQ_dim(0),
    m_ACQ_size(0),
    m_PVM_matrix(0),
    m_PVM_AntiAlias(0),
    m_NI(0),
    m_ACQ_obj_order(0),
    m_NSLICES(0),
    m_ACQ_n_echo_images(0),
    m_ACQ_phase_factor(0),
    m_ACQ_rare_factor(0),
    m_NR(0),
    m_ACQ_spatial_size_1(0),
    m_ACQ_spatial_phase_1(0),
    m_ACQ_spatial_size_2(0),
    m_ACQ_spatial_phase_2(0),
    m_spatial_phase_1_min(1.0),
    m_spatial_phase_1_max(-1.0),
    m_spatial_phase_2_min(1.0),
    m_spatial_phase_2_max(-1.0),
    m_ky_profile_order_steps(0),
    m_kz_profile_order_steps(0),
    m_ky_profile_order(0),
    m_kz_profile_order(0),
    m_ky_min(0),
    m_ky_max(0),
    m_kz_min(0),
    m_kz_max(0),
    m_1k_file_format(false)
{

}

BrukerProfileListGenerator::~BrukerProfileListGenerator()
{
  if (m_ACQ_size) delete [] m_ACQ_size;
  if (m_ACQ_obj_order) delete [] m_ACQ_obj_order;
  if (m_ACQ_spatial_phase_1) delete [] m_ACQ_spatial_phase_1;
  if (m_ACQ_spatial_phase_2) delete [] m_ACQ_spatial_phase_2;
  if (m_ky_profile_order) delete [] m_ky_profile_order;
  if (m_kz_profile_order) delete [] m_kz_profile_order;
}

void BrukerProfileListGenerator::ExtractParametersFromAcq(BrukerParameterFile* acqp, BrukerParameterFile* method)
{
  if (!acqp) {
    return;
  }

  /* Get the necessary information from the acqp file */
  BrukerParameter* p = 0;

  p = acqp->FindParameter(std::string("ACQ_dim"));
  if (p) {
    m_ACQ_dim = p->GetValue()->GetIntValue();
    m_ACQ_size = new int[m_ACQ_dim];
    if (!m_ACQ_size) {
      std::cerr << " BrukerProfileListGenerator: Unable to allocate array for ACQ_size" << std::endl;
      return;
    }
  }
  
  p = acqp->FindParameter(std::string("ACQ_size"));
  if (p) {
    if (p->GetNumberOfValues() != m_ACQ_dim) {
      std::cerr << "BrukerProfileListGenerator: Mismatch between ACQ_dim and length of ACQ_size" << std::endl;
      return;
    }
    for (int i = 0; i < p->GetNumberOfValues(); i++) {
      m_ACQ_size[i] = p->GetValue(i)->GetIntValue();
    }
  }

  if (method) {
    p = method->FindParameter(std::string("PVM_EncAvailReceivers"));
    if (p) {
      m_NumChannels = p->GetValue(0)->GetIntValue();
    }

    m_PVM_matrix = new int[m_ACQ_dim];
    m_PVM_AntiAlias = new float[m_ACQ_dim];
    if (!m_PVM_matrix || !m_PVM_AntiAlias) {
      std::cerr << " BrukerProfileListGenerator: Unable to allocate array for PVM_matrix and PVM_AntiAlias" << std::endl;
      return;
    }
    p = method->FindParameter(std::string("PVM_Matrix"));
    if (p) {
      if (p->GetNumberOfValues() != m_ACQ_dim) {
	std::cerr << "BrukerProfileListGenerator: Mismatch between ACQ_dim and length of PVM_matrix" << std::endl;
	return;
      }
      for (int i = 0; i < p->GetNumberOfValues(); i++) {
	m_PVM_matrix[i] = p->GetValue(i)->GetIntValue();
      }
    }
    p = method->FindParameter(std::string("PVM_AntiAlias"));
    if (p) {
      if (p->GetNumberOfValues() != m_ACQ_dim) {
	std::cerr << "BrukerProfileListGenerator: Mismatch between ACQ_dim and length of PVM_AntiAlias" << std::endl;
	return;
      }
      for (int i = 0; i < p->GetNumberOfValues(); i++) {
	m_PVM_AntiAlias[i] = p->GetValue(i)->GetFloatValue();
      }
    }
  }

  p = acqp->FindParameter(std::string("NI"));
  if (p) {
    m_NI = p->GetValue()->GetIntValue();
    m_ACQ_obj_order = new int[m_NI];
    if (!m_ACQ_obj_order) {
      std::cerr << " BrukerProfileListGenerator: Unable to allocate array for ACQ_obj_order" << std::endl;
      return;
    }
  }

  p = acqp->FindParameter(std::string("ACQ_obj_order"));
  if (p) {
    if (p->GetNumberOfValues() != m_NI) {
      std::cerr << "BrukerProfileListGenerator: Mismatch between NI and length of ACQ_obj_order" << std::endl;
      return;
    }
    for (int i = 0; i < m_NI; i++) {
      m_ACQ_obj_order[i] = p->GetValue(i)->GetIntValue();
    }

  }

  p = acqp->FindParameter(std::string("NSLICES"));
  if (p) {
    m_NSLICES = p->GetValue()->GetIntValue();
  }

  p = acqp->FindParameter(std::string("ACQ_n_echo_images"));
  if (p) {
    m_ACQ_n_echo_images = p->GetValue()->GetIntValue();
  }
  
  p = acqp->FindParameter(std::string("ACQ_phase_factor"));
  if (p) {
    m_ACQ_phase_factor = p->GetValue()->GetIntValue();
  }

  p = acqp->FindParameter(std::string("ACQ_rare_factor"));
  if (p) {
    m_ACQ_rare_factor = p->GetValue()->GetIntValue();
  }

  p = acqp->FindParameter(std::string("NR"));
  if (p) {
    m_NR = p->GetValue()->GetIntValue();
  }

  p = acqp->FindParameter(std::string("ACQ_spatial_size_1"));
  if (p) {
    m_ACQ_spatial_size_1 = p->GetValue()->GetIntValue();
  }
  p = acqp->FindParameter(std::string("ACQ_spatial_size_2"));
  if (p) {
    m_ACQ_spatial_size_2 = p->GetValue()->GetIntValue();
  }

  if (m_ACQ_spatial_size_1) {
      p = acqp->FindParameter(std::string("ACQ_spatial_phase_1"));
      if (p) {
	m_ACQ_spatial_phase_1 = new float[m_ACQ_spatial_size_1];
	if (!m_ACQ_spatial_phase_1) {
	  std::cerr << " BrukerProfileListGenerator: Unable to allocate array for ACQ_spatial_phase_1" << std::endl;
	  return;
	}
	
	for (int i = 0; i < m_ACQ_spatial_size_1; i++) {
	  m_ACQ_spatial_phase_1[i] = p->GetValue(i)->GetFloatValue();
	  if (m_ACQ_spatial_phase_1[i] > m_spatial_phase_1_max) m_spatial_phase_1_max = m_ACQ_spatial_phase_1[i];
	  if (m_ACQ_spatial_phase_1[i] < m_spatial_phase_1_min) m_spatial_phase_1_min = m_ACQ_spatial_phase_1[i];
	}
      }
  }

  if (m_ACQ_spatial_size_2) {
      p = acqp->FindParameter(std::string("ACQ_spatial_phase_2"));
      if (p) {
	m_ACQ_spatial_phase_2 = new float[m_ACQ_spatial_size_2];
	if (!m_ACQ_spatial_phase_2) {
	  std::cerr << " BrukerProfileListGenerator: Unable to allocate array for ACQ_spatial_phase_2" << std::endl;
	  return;
	}
	
	for (int i = 0; i < m_ACQ_spatial_size_2; i++) {
	  m_ACQ_spatial_phase_2[i] = p->GetValue(i)->GetFloatValue();
	  if (m_ACQ_spatial_phase_2[i] > m_spatial_phase_2_max) m_spatial_phase_2_max = m_ACQ_spatial_phase_2[i];
	  if (m_ACQ_spatial_phase_2[i] < m_spatial_phase_2_min) m_spatial_phase_2_min = m_ACQ_spatial_phase_2[i];
	}
      }
  }

  p = acqp->FindParameter(std::string("GO_block_size"));
  if (p) {
    if (!p->GetValue()->GetStringValue().compare(std::string("Standard_KBlock_Format"))) {
      m_1k_file_format = true;
    }
  }

  p = acqp->FindParameter(std::string("GO_raw_data_format"));
  if (p) {
    if (!p->GetValue()->GetStringValue().compare(std::string("GO_16BIT_SGN_INT"))) {
      m_data_format = BrukerRawDataProfile::GO_16BIT_SGN_INT;
    } else if (!p->GetValue()->GetStringValue().compare(std::string("GO_32BIT_SGN_INT"))) {
      m_data_format = BrukerRawDataProfile::GO_32BIT_SGN_INT;
    } else if (!p->GetValue()->GetStringValue().compare(std::string("GO_32BIT_FLOAT"))) {
      m_data_format = BrukerRawDataProfile::GO_32BIT_FLOAT;
    } else {
      m_data_format = BrukerRawDataProfile::GO_FORMAT_NONE;
    }
  }

  /* Some derived parameters */
  if (!method) {
    std::cout<<"Running without method file" << std::endl;
    if (m_ACQ_spatial_phase_1) {
      m_ky_profile_order_steps = m_ACQ_spatial_size_1;
      m_ky_profile_order = new int[m_ky_profile_order_steps];
      if (!m_ky_profile_order) {
	std::cerr << "BrukerProfileListGenerator: Unable to allocate memory for ky profile order" << std::endl;
	return;
      }
      for (int i = 0; i < m_ACQ_spatial_size_1; i++) {
	m_ky_profile_order[i] = 
	  static_cast<int>(m_ACQ_spatial_phase_1[i]/(m_spatial_phase_1_max-m_spatial_phase_1_min)*(m_ACQ_size[1]));
	if (m_ky_profile_order[i] < m_ky_min) m_ky_min = m_ky_profile_order[i];
	if (m_ky_profile_order[i] > m_ky_max) m_ky_max = m_ky_profile_order[i];
      }
    } else {
      /* TODO: We need to determine the profile order when not defined by spatial phase */
      if (m_ACQ_dim > 1) {
	std::cerr << "BrukerProfileListGenerator: No defined ACQ_spatial_phase_1" << std::endl;
      }
    }
  } else {
    p = method->FindParameter(std::string("PVM_EncSteps1"));
    if (p) {
      m_ky_profile_order_steps = p->GetNumberOfValues();
      m_ky_profile_order = new int[m_ky_profile_order_steps];
      if (!m_ky_profile_order) {
	std::cerr << "BrukerProfileListGenerator: Unable to allocate memory for ky profile order" << std::endl;
	return;
      }
      for (int i = 0; i < m_ky_profile_order_steps; i++) {
	m_ky_profile_order[i] = p->GetValue(i)->GetIntValue();
	if (m_ky_profile_order[i] < m_ky_min) m_ky_min = m_ky_profile_order[i];
	if (m_ky_profile_order[i] > m_ky_max) m_ky_max = m_ky_profile_order[i];
      }      
    }
  }

  if (!method) {
    if (m_ACQ_spatial_phase_2) {
      m_kz_profile_order = new int[m_ACQ_spatial_size_2];
      if (!m_kz_profile_order) {
	std::cerr << "BrukerProfileListGenerator: Unable to allocate memory for kz profile order" << std::endl;
	return;
      }
      for (int i = 0; i < m_ACQ_spatial_size_2; i++) {
	m_kz_profile_order[i] = 
	  static_cast<int>(m_ACQ_spatial_phase_2[i]/(m_spatial_phase_2_max-m_spatial_phase_2_min)*(m_ACQ_size[1]));
	if (m_kz_profile_order[i] < m_kz_min) m_kz_min = m_kz_profile_order[i];
	if (m_kz_profile_order[i] > m_kz_max) m_kz_max = m_kz_profile_order[i];

      }
    } else {
      /* TODO: We need to determine the profile order when not defined by spatial phase */
      if (m_ACQ_dim > 2) {
	std::cerr << "BrukerProfileListGenerator: No defined ACQ_spatial_phase_2" << std::endl;
      }
    }
  } else {
    p = method->FindParameter(std::string("PVM_EncSteps2"));
    if (p) {
      m_kz_profile_order_steps = p->GetNumberOfValues();
      m_kz_profile_order = new int[m_kz_profile_order_steps];
      if (!m_kz_profile_order) {
	std::cerr << "BrukerProfileListGenerator: Unable to allocate memory for ky profile order" << std::endl;
	return;
      }
      for (int i = 0; i < m_kz_profile_order_steps; i++) {
	m_kz_profile_order[i] = p->GetValue(i)->GetIntValue();
	if (m_kz_profile_order[i] < m_kz_min) m_kz_min = m_kz_profile_order[i];
	if (m_kz_profile_order[i] > m_kz_max) m_kz_max = m_kz_profile_order[i];
      }      
    }
  }
}

BrukerRawDataProfile* BrukerProfileListGenerator::GetProfileList(BrukerParameterFile* acqp, BrukerParameterFile* method)
{
  
  ExtractParametersFromAcq(acqp, method);
  //PrintParameters();

  BrukerRawDataProfile* first   = 0;
  BrukerRawDataProfile* current = 0;
  
  unsigned long int position = 0;

  unsigned long int profile_data_length = 0;
  if (m_1k_file_format) {
    int data_size;
    if (m_data_format == BrukerRawDataProfile::GO_32BIT_SGN_INT || m_data_format == BrukerRawDataProfile::GO_32BIT_FLOAT) {
      data_size = 4;
    } else {
      data_size = 2;
    }
    profile_data_length = m_ACQ_size[0]*m_NumChannels*data_size;
    if (profile_data_length % 1024) {
      profile_data_length = ((profile_data_length / 1024)+1)*1024;
    }
  }

  for (int nr = 0; nr < m_NR; nr++) { /* Repeated measurements */
    for (int e2 = 0; e2 < ((m_ACQ_dim <= 2) ? 1 : m_ACQ_size[2]); e2++) {
      for (int e1 = 0; e1 < ((m_ACQ_dim <= 1) ? 1 : (m_ACQ_size[1]/m_ACQ_phase_factor)); e1++) {
	for (int ns = 0; ns < m_NSLICES; ns++) {
	  for (int ph = 0; ph < m_ACQ_phase_factor; ph++) {
            for (int ne = 0; ne < m_ACQ_n_echo_images; ne++) {
              BrukerRawDataProfile* new_p = new BrukerRawDataProfile();  
              if (!first) {
                first = new_p;
              } else {
                current->SetNext(new_p);
                new_p->SetPrevious(current);
              }
              current = new_p;
	    
              current->SetProfileLength(m_ACQ_size[0]/2);
              current->SetNumberOfChannels(m_NumChannels);
              if (m_ACQ_spatial_size_1 > 0) {
                current->SetEncodeStep1(m_ky_profile_order[e1*m_ACQ_phase_factor+ph]);
              } 
              if (m_ACQ_spatial_size_2 > 0) {
                current->SetEncodeStep2(m_kz_profile_order[e2]);
              } 
	    
              /* TODO: Deal with multichannel data */
              //current->SetChannelNo(0);
	    
              current->SetSliceNo(m_ACQ_obj_order[ns]);
	    
              current->SetEchoNo(ne);
            
              current->SetRepetitionNo(nr);
	    
              current->SetDataFormat(m_data_format);

              current->SetFilePosition(position);
              position += profile_data_length;
            }	    
	  }
	}
      }
    } 

  }

  return first;
}

void BrukerProfileListGenerator::PrintParameters()
{
  std::cout << "ACQ_dim: " << m_ACQ_dim << std::endl;

  std::cout << "ACQ_size[" << m_ACQ_dim << "]: ";
  for (int i = 0; i < m_ACQ_dim; i++) {
    std::cout << m_ACQ_size[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "NI: " << m_NI << std::endl;

  std::cout << "ACQ_obj_order[" << m_NI << "]: ";
  for (int i = 0; i < m_NI; i++) {
    std::cout << m_ACQ_obj_order[i] << " ";
  }
  std::cout << std::endl;
  
  std::cout << "NSLICES: " << m_NSLICES << std::endl;
  std::cout << "ACQ_n_echo_images: " << m_ACQ_n_echo_images << std::endl;
  
  std::cout << "ACQ_phase_factor: " << m_ACQ_phase_factor << std::endl;
  std::cout << "ACQ_rare_factor: " << m_ACQ_rare_factor << std::endl;
  std::cout << "NR: " << m_NR << std::endl;

  std::cout << "ACQ_spatial_size_1: " << m_ACQ_spatial_size_1 << std::endl;

  std::cout << "ACQ_spatial_phase_1[" << m_ACQ_spatial_size_1 << "]: ";
  for (int i = 0; i < m_ACQ_spatial_size_1; i++) {
    std::cout << m_ACQ_spatial_phase_1[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "ACQ_spatial_size_2: " << m_ACQ_spatial_size_2 << std::endl;

  std::cout << "ACQ_spatial_phase_2[" << m_ACQ_spatial_size_2 << "]: ";
  for (int i = 0; i < m_ACQ_spatial_size_2; i++) {
    std::cout << m_ACQ_spatial_phase_2[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "spatial_phase_1_min: " << m_spatial_phase_1_min << std::endl;
  std::cout << "spatial_phase_1_max: " << m_spatial_phase_1_max << std::endl;
  std::cout << "spatial_phase_2_min: " << m_spatial_phase_2_min << std::endl;
  std::cout << "spatial_phase_2_max: " << m_spatial_phase_2_max << std::endl;

  std::cout << "ky_profile_order[" << m_ky_profile_order_steps << "]: ";
  for (int i = 0; i < m_ky_profile_order_steps; i++) {
    std::cout << m_ky_profile_order[i] << " ";
  }
  std::cout << std::endl;
  
  std::cout << "kz_profile_order[" << m_kz_profile_order_steps << "]: ";
  for (int i = 0; i < m_kz_profile_order_steps; i++) {
    std::cout << m_kz_profile_order[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "1k_file_format: " << (m_1k_file_format ? "true" : "false") << std::endl;
}

int BrukerProfileListGenerator::GetDimensionSize(int dimension) 
{
  if (dimension >= m_ACQ_dim || dimension < 0) return 0;

  //If we have a method file
  if (m_PVM_matrix && m_PVM_AntiAlias) {
    return static_cast<int>(m_PVM_matrix[dimension]*m_PVM_AntiAlias[dimension]);
  }

  //If not, do the best we can with what we have got
  if (dimension ==  0) return m_ACQ_size[0]/2;
  return m_ACQ_size[dimension];

}

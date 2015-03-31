#ifndef BRUKER_RAWDATA_HPP
#define BRUKER_RAWDATA_HPP

#include "brukerparameterparser.hpp"
#include "ndarray.hpp"
#include "types.hpp"

#include <fstream>

//#define MAX_READ_BUFFER 20480


class BrukerRawDataProfile {
public:

  typedef enum {
    GO_FORMAT_NONE = 0,
    GO_16BIT_SGN_INT,
    GO_32BIT_SGN_INT,
    GO_32BIT_FLOAT,
    GO_DATA_FORMAT_MAX
  } BrukerDataFormat;


  BrukerRawDataProfile();
  ~BrukerRawDataProfile();

  void SetProfileLength(unsigned int);
  unsigned int GetProfileLength();

  void SetNumberOfChannels(unsigned int);
  unsigned int GetNumberOfChannels();
    
  void SetEncodeStep1(int e);
  int GetEncodeStep1();

  void SetEncodeStep2(int e);
  int GetEncodeStep2();

  void SetChannelNo(unsigned int);
  unsigned int GetChannelNo();

  void SetObjectNo(unsigned int);
  unsigned int GetObjectNo();

  void SetSliceNo(unsigned int);
  unsigned int GetSliceNo();

  void SetEchoNo(unsigned int);
  unsigned int GetEchoNo();

  void SetRepetitionNo(unsigned int);
  unsigned int GetRepetitionNo();

  void SetFilePosition(unsigned long int p);
  unsigned long int GetFilePosition();

  void SetNext(BrukerRawDataProfile* p);
  BrukerRawDataProfile* GetNext();

  void SetPrevious(BrukerRawDataProfile* p);
  BrukerRawDataProfile* GetPrevious();

  void SetDataFormat(BrukerDataFormat f);
  BrukerDataFormat GetDataFormat();

  void ReadData(std::ifstream& fs);

  void WriteData(std::ofstream& fs, float max_val);

  void SetRawData(float* d);

  void SetRawDataFromArray(mr_recon::ComplexFloatArray& a, int ky_min = -10000, int kz_min = -10000);

  void DeleteLinkedProfiles();

  void DeleteNext();

  void DeletePrevious();

  BrukerRawDataProfile* SortLinkedProfilesByFilePosition();

  float GetMaxDataValue(bool include_all_linked_profiles = true);

  float* GetDataPtr() { return m_pData; }

protected:
  unsigned int m_uiProfileLength;
  unsigned int m_uiNumberOfChannels;
  int m_iEncodeStep1;    /* ky */
  int m_iEncodeStep2;    /* kz */
  unsigned int m_uiChannelNo;   
  unsigned int m_uiObjectNo;       /* of NI */
  unsigned int m_uiSliceNo;         /* of NSLICES */    
  unsigned int m_uiEchoNo;         /* of ACQ_n_echo_images */    
  unsigned int m_uiRepetitionNo;   /* of NR */

  unsigned long int m_ulFilePosition;

  float* m_pData;

  BrukerRawDataProfile* m_pNext;
  BrukerRawDataProfile* m_pPrevious;

  BrukerDataFormat m_DataFormat;
  
  void DeAllocateMemory();

  /* Some read buffers, these are declared static to avoid allocation and deallocation in all profile instances */
  /* CAVE: This would not be thread safe!                                                                       */
  //static short s_ShortBuffer[MAX_READ_BUFFER];
  //static int s_IntBuffer[MAX_READ_BUFFER];

};


class BrukerProfileListGenerator
{

public:
  BrukerProfileListGenerator();
  ~BrukerProfileListGenerator();

  BrukerRawDataProfile* GetProfileList(BrukerParameterFile* acqp, BrukerParameterFile* method = 0);
  
  void PrintParameters();

  int GetNumberOfChannels() { return m_NumChannels; }
  int GetNumberOfDimensions() { return m_ACQ_dim; }
  int GetDimensionSize(int dimension = 0);
  int GetNumberOfObjects() {return m_NI;}
  int GetNumberOfSlices() {return m_NSLICES;}
  int GetNumberOfEchos() {return  m_ACQ_n_echo_images;}
  int GetNumberOfRepetitions() { return m_NR; }
  int GetMinEncodingStep1() { return m_ky_min; }
  int GetMaxEncodingStep1() { return m_ky_max; }
  int GetMinEncodingStep2() { return m_kz_min; }
  int GetMaxEncodingStep2() { return m_kz_max; }

protected:
  void ExtractParametersFromAcq(BrukerParameterFile* acqp, BrukerParameterFile* method = 0);
  
  int  m_ACQ_dim;
  int* m_ACQ_size;
  int  m_NumChannels;
  int* m_PVM_matrix;
  float* m_PVM_AntiAlias;
  int  m_NI;
  int* m_ACQ_obj_order;
  int  m_NSLICES;
  int  m_ACQ_n_echo_images;
  int  m_ACQ_phase_factor;
  int  m_ACQ_rare_factor;
  int  m_NR;
  int  m_ACQ_spatial_size_1;
  float* m_ACQ_spatial_phase_1;
  int  m_ACQ_spatial_size_2;
  float* m_ACQ_spatial_phase_2;

  float m_spatial_phase_1_min;
  float m_spatial_phase_1_max;
  float m_spatial_phase_2_min;
  float m_spatial_phase_2_max;

  int  m_ky_profile_order_steps;
  int  m_kz_profile_order_steps;
  int* m_ky_profile_order;
  int* m_kz_profile_order;
  int m_ky_min;
  int m_ky_max;
  int m_kz_min;
  int m_kz_max;
  bool m_1k_file_format;
  BrukerRawDataProfile::BrukerDataFormat m_data_format;
};


#endif //BRUKER_RAWDATA_HPP

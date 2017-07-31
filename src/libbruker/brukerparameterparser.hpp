/*****************************************************
 *
 *  Class for parsing Bruker parameter files
 *
 *  It uses the flex tool for lexical analysis
 *
 *
 *  Michael S. Hansen (michael.hansen@nih.gov)
 *
 *  2009.06.24 Initial Revision
 *  2015.03.26 Moved to bruker_to_ismrmrd
 *
 *****************************************************/

#ifndef BRUKERPARAMETERPARSER_HPP
#define BRUKERPARAMETERPARSER_HPP

#undef yyFlexLexer
#include <FlexLexer.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <climits>
#include <cstdlib>

enum {VARIABLE_START = 1,
      VIS_START,
      INFO_START,
      VARIABLE_NAME,
      VARIABLE_SIZE,
      STRING_VALUE,
      LABEL_VALUE,
      FLOAT_VALUE,
      INTEGER_VALUE,
      MULTI_VALUE_START,
      MULTI_VALUE_END,
      INFO_VALUE,
      VISU_VALUE,
      SOMETHING_ELSE};


class BrukerParameterValue {

public:
  enum PARAMETER_VALUE_TYPE {PARAM_VALUE_TYPE_NONE = 0,
			     PARAM_VALUE_TYPE_STRING,
			     PARAM_VALUE_TYPE_INT,
			     PARAM_VALUE_TYPE_FLOAT,
			     PARAM_VALUE_TYPE_LABEL};

  BrukerParameterValue();
  ~BrukerParameterValue();
  
  BrukerParameterValue* GetNext() { return m_pNextValue; }
  void SetNext(BrukerParameterValue* pNext) { m_pNextValue = pNext; }

  BrukerParameterValue* GetPrevious() { return m_pNextValue; }
  void SetPrevious(BrukerParameterValue* pPrevious) { m_pPreviousValue = pPrevious; }

  std::string GetStringValue() { return m_StringValue; }
  long int GetIntValue() { return m_IntValue; }
  double GetFloatValue() { return m_FloatValue; }

  void SetStringValue(std::string v, bool append_if_set);
  void SetLabelValue(std::string v, bool append_if_set);
  void SetIntValue(std::string v, bool append_if_set);
  void SetFloatValue(std::string v, bool append_if_set);

  BrukerParameterValue* GetLastParameterValueInChain();
  BrukerParameterValue* GetFirstParameterValueInChain();
  unsigned int GetNumberOfParameterValuesInChain();
  BrukerParameterValue* GetParameterValueInChain(unsigned int index);

  void PrintValue(std::ostream &s);

protected:
  BrukerParameterValue* m_pNextValue;
  BrukerParameterValue* m_pPreviousValue;
  double m_FloatValue;
  long int m_IntValue;
  std::string m_StringValue;
  int m_ValueType;

};

class BrukerParameter {

public:
  enum PARAMETER_TYPE {PARAM_TYPE_NONE = 0,
		       PARAM_TYPE_PARAM,
		       PARAM_TYPE_INFO,
		       PARAM_TYPE_VIS};
  BrukerParameter();
  ~BrukerParameter();

  BrukerParameter* GetNextParameter() { return m_pNextParameter; }
  void SetNextParameter(BrukerParameter* n) { m_pNextParameter = n; }

  BrukerParameter* GetPreviousParameter() { return m_pPreviousParameter; }
  void SetPreviousParameter(BrukerParameter* n) { m_pPreviousParameter = n; }

  void SetName(std::string n) { m_ParameterName = n; }

  std::string GetName() { return m_ParameterName; }
  
  void AppendDimension(int d) { m_dimensions.push_back(d); }

  void NewStringValue(std::string v);

  void NewLabelValue(std::string v);

  void NewIntValue(std::string v);

  void NewFloatValue(std::string v);

  void NewIntValue(int vi);

  void NewFloatValue(float vf);

  void SetMultiValueMode(bool on);

  void PrintParameter(std::ostream &s);

  void PrintValues(std::ostream &s);

  void SetParameterType(int ptype) { m_ParameterType = ptype; }
  
  int GetParameterType() { return m_ParameterType; }

  int GetNumberOfValues() { return m_NumValues; }

  BrukerParameterValue* GetValue(unsigned int val_no = 0);

  void AddParameterBefore(BrukerParameter* p);

  void AddParameterAfter(BrukerParameter* p);

  void DeleteParameterFromChain();

  std::vector<unsigned int> GetDimensions() {return m_dimensions;} 

protected:
  std::vector<unsigned int> m_dimensions;
  std::string m_ParameterName;
  BrukerParameterValue** m_pValues;
  BrukerParameter* m_pNextParameter;
  BrukerParameter* m_pPreviousParameter;

  bool m_bMultiValueModeOn;
  int m_NumValues;
  int m_CurrentValueNum;
  int m_ParameterType;

  void AllocateValuesArray(bool firstDimIsString = false);
  void DeleteValuesArray();

};

class BrukerParameterFile {

public:
  BrukerParameterFile(std::string filename);
  ~BrukerParameterFile();

  void PrintFile(std::ostream &s);

  BrukerParameter* FindParameter(std::string name);

protected:
  int ParseFile();

  yyFlexLexer* m_pLexer;
  std::string  m_FileName;
  std::istream* m_pInputStream;
  std::filebuf* m_pInputFileBuffer;

  BrukerParameter* m_pFirstParameter;
  BrukerParameter* m_pCurrentParameter;
};

#endif //BRUKERPARAMETERPARSER_HPP


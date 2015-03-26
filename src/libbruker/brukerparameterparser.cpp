/*********************************************************
 *
 * Implementation of BrukerParameterparser
 *
 *
 * Michael S. Hansen (michael.hansen@nih.gov)
 *
 * 2009.06.24 Initial Revision
 * 2015.03.26 Moved to bruker_to_ismrmrd
 *
 ********************************************************/

#include "brukerparameterparser.hpp"

#include <sstream>
#include <iomanip>

BrukerParameterFile::BrukerParameterFile(std::string filename)
  : m_pLexer(0),
    m_pInputStream(0),
    m_pInputFileBuffer(0),
    m_pFirstParameter(0),
    m_pCurrentParameter(0)
{
  m_FileName = filename;

  m_pInputFileBuffer = new std::filebuf();

  if (m_pInputFileBuffer == 0) {
    std::cerr << "BrukerParameterFile: failed to allocate input file buffer" << std::endl;
    exit(-1);
  }
  
  m_pInputFileBuffer->open(m_FileName.c_str(), std::ios::in);

  m_pInputStream = new std::istream(m_pInputFileBuffer);
  if (m_pInputStream == 0) {
    std::cerr << "BrukerParameterFile: failed to initiate input stream" << std::endl;
    exit(-1);
  }

  m_pLexer = new yyFlexLexer(m_pInputStream);
  if (m_pLexer == 0) {
    std::cerr << "BrukerParameterFile: failed to allocate lexer" << std::endl;
    exit(-1);
  }

  ParseFile();
}


BrukerParameterFile::~BrukerParameterFile()
{
  if (m_pInputStream) {
    delete m_pInputStream;
  }

  if (m_pInputFileBuffer) {
    m_pInputFileBuffer->close();
    delete m_pInputFileBuffer;
  }
  
}

int BrukerParameterFile::ParseFile()
{
  int token;
  std::string tmp_str;

  if (!m_pLexer) {
    std::cerr << "BrukerParameterFile: error, lexer not defined" << std::endl;
    return -1;
  }

  while ((token = m_pLexer->yylex()))
  {
    switch (token) {

    case VARIABLE_START:
      if (!m_pCurrentParameter) {
	m_pCurrentParameter = new BrukerParameter();
      } else {
	BrukerParameter* tmp = m_pCurrentParameter;
	m_pCurrentParameter = new BrukerParameter();
	tmp->SetNextParameter(m_pCurrentParameter);
	m_pCurrentParameter->SetPreviousParameter(tmp);
      }

      if (!m_pFirstParameter) {
	m_pFirstParameter = m_pCurrentParameter;
      }

      m_pCurrentParameter->SetParameterType(BrukerParameter::PARAM_TYPE_PARAM);
      break;

    case VIS_START:
      if (!m_pCurrentParameter) {
	m_pCurrentParameter = new BrukerParameter();
      } else {
	BrukerParameter* tmp = m_pCurrentParameter;
	m_pCurrentParameter = new BrukerParameter();
	tmp->SetNextParameter(m_pCurrentParameter);
	m_pCurrentParameter->SetPreviousParameter(tmp);
      }

      if (!m_pFirstParameter) {
	m_pFirstParameter = m_pCurrentParameter;
      }

      m_pCurrentParameter->SetParameterType(BrukerParameter::PARAM_TYPE_VIS);

      break;

    case VISU_VALUE:
      tmp_str = m_pLexer->YYText();
      m_pCurrentParameter->NewLabelValue(tmp_str.substr(0,tmp_str.length()-1));
      break;


    case INFO_START:
      if (!m_pCurrentParameter) {
	m_pCurrentParameter = new BrukerParameter();
      } else {
	BrukerParameter* tmp = m_pCurrentParameter;
	m_pCurrentParameter = new BrukerParameter();
	tmp->SetNextParameter(m_pCurrentParameter);
	m_pCurrentParameter->SetPreviousParameter(tmp);
      }

      if (!m_pFirstParameter) {
	m_pFirstParameter = m_pCurrentParameter;
      }

      m_pCurrentParameter->SetParameterType(BrukerParameter::PARAM_TYPE_INFO);
      break;

    case INFO_VALUE:
      tmp_str = m_pLexer->YYText();
      m_pCurrentParameter->NewLabelValue(tmp_str.substr(0,tmp_str.length()-1));
      break;

    case VARIABLE_NAME:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->SetName(std::string(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set variable name of null variable." << std::endl;
	exit(-1);
      }
      break;

    case VARIABLE_SIZE:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->AppendDimension(atoi(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set variable dimension size of null variable." << std::endl;
	exit(-1);
      }
      break;

    case STRING_VALUE:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->NewStringValue(std::string(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set string value of null variable." << std::endl;
	exit(-1);
      }      
      break;

    case LABEL_VALUE:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->NewLabelValue(std::string(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set label value of null variable." << std::endl;
	exit(-1);
      }      
      break;

    case FLOAT_VALUE:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->NewFloatValue(std::string(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set float value of null variable." << std::endl;
	exit(-1);
      }      
      break;

    case INTEGER_VALUE:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->NewIntValue(std::string(m_pLexer->YYText()));
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set int value of null variable." << std::endl;
	exit(-1);
      }      
      break;

    case MULTI_VALUE_START:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->SetMultiValueMode(true);
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set multi value mode of null variable." << std::endl;
	exit(-1);
      }            
      break;

    case MULTI_VALUE_END:
      if (m_pCurrentParameter) {
	m_pCurrentParameter->SetMultiValueMode(false);
      } else {
	std::cerr << "BrukerParameterFile: Error trying to set multi value mode of null variable." << std::endl;
	exit(-1);
      }            
      break;

    }
    
    //std::cout << "Token: " << token << ", " << m_pLexer->YYText() << std::endl;
  }

  //PrintFile(std::cout);

  return 1;  //printf("# of lines = %d, # of chars = %d\n", num_lines, num_chars);
}

void BrukerParameterFile::PrintFile(std::ostream &s)
{
  BrukerParameter* first = m_pFirstParameter;
  while (first) {
    first->PrintParameter(s);
    first = first->GetNextParameter();
  }

}

BrukerParameter* BrukerParameterFile::FindParameter(std::string name)
{
  BrukerParameter* result = 0;
  BrukerParameter* first = m_pFirstParameter;
  while (first && !result) {
    if (first->GetName().compare(name) == 0) {
      result = first;
      break;
    }
    first = first->GetNextParameter();
  }  

  return result;
}

BrukerParameter::BrukerParameter()
  : m_ParameterName(""),
    m_pValues(0),
    m_pNextParameter(0),
    m_pPreviousParameter(0),
    m_bMultiValueModeOn(false),
    m_NumValues(0),
    m_CurrentValueNum(0),
    m_ParameterType(PARAM_TYPE_NONE)
{


}


BrukerParameter::~BrukerParameter()
{
  DeleteValuesArray();
}

void BrukerParameter::NewStringValue(std::string s)
{
  if (!m_pValues) AllocateValuesArray(true);
  if (m_CurrentValueNum < m_NumValues) {
    if (!m_pValues[m_CurrentValueNum]) m_pValues[m_CurrentValueNum] = new BrukerParameterValue();
    m_pValues[m_CurrentValueNum]->SetStringValue(s.substr(1,s.size()-2), true);
    if (!m_bMultiValueModeOn) m_CurrentValueNum++;
  } else {
    std::cerr << "BrukerParameter (" << m_ParameterName << "): Too many values (string) received. No more space in array" << std::endl;
  }
}

void BrukerParameter::NewLabelValue(std::string v)
{
  if (!m_pValues) AllocateValuesArray();
  if (m_CurrentValueNum < m_NumValues) {
    if (!m_pValues[m_CurrentValueNum]) m_pValues[m_CurrentValueNum] = new BrukerParameterValue();
    m_pValues[m_CurrentValueNum]->SetLabelValue(v, true);
    if (!m_bMultiValueModeOn) m_CurrentValueNum++;
  } else {
    std::cerr << "BrukerParameter (" << m_ParameterName << "): Too many values (label) received. No more space in array" << std::endl;
  }
}

void BrukerParameter::NewIntValue(int vi)
{
  std::stringstream st;
  std::string v;
  st << vi;
  st >> v;
  NewIntValue(v);
}

void BrukerParameter::NewFloatValue(float vf)
{
  std::stringstream st;
  std::string v;
  st << vf;
  st >> v;
  NewFloatValue(v);
}


void BrukerParameter::NewIntValue(std::string v)
{
  if (!m_pValues) AllocateValuesArray();
  if (m_CurrentValueNum < m_NumValues) {
    if (!m_pValues[m_CurrentValueNum]) m_pValues[m_CurrentValueNum] = new BrukerParameterValue();
    m_pValues[m_CurrentValueNum]->SetIntValue(v, true);
    if (!m_bMultiValueModeOn) m_CurrentValueNum++;
  } else {
    std::cerr << "BrukerParameter (" << m_ParameterName << "): Too many values (int) received. No more space in array" << std::endl;
  }
}

void BrukerParameter::NewFloatValue(std::string v)
{
  if (!m_pValues) AllocateValuesArray();
  if (m_CurrentValueNum < m_NumValues) {
    if (!m_pValues[m_CurrentValueNum]) m_pValues[m_CurrentValueNum] = new BrukerParameterValue();
    m_pValues[m_CurrentValueNum]->SetFloatValue(v, true);
    if (!m_bMultiValueModeOn) m_CurrentValueNum++;
  } else {
    std::cerr << "BrukerParameter (" << m_ParameterName << "): Too many values (float) received. No more space in array" << std::endl;
  }
}

void BrukerParameter::AllocateValuesArray(bool firstDimIsString) {
  m_NumValues = 1;

  for (unsigned int c = 0; c < m_dimensions.size(); c++) {
    m_NumValues *= m_dimensions[c];
  }

  if (firstDimIsString) {
    if (m_dimensions.size() == 1) {
      m_NumValues /= m_dimensions[0];
    } else if (m_dimensions.size() == 2) {
      m_NumValues /= m_dimensions[1];
    }
  }

  m_pValues = new BrukerParameterValue*[m_NumValues];
  if (m_pValues == 0) {
    std::cerr << "BrukerParameter: Unable to allocate array for parameter values." << std::endl;
    exit(-1);
  }

  for (int i = 0; i < m_NumValues; i++) m_pValues[i] = 0;						
}

void BrukerParameter::DeleteValuesArray()
{
  if (m_pValues) {
    for (int i = 0; i < m_NumValues; i++) {
      if (m_pValues[i]) {
	delete m_pValues[i];
	m_pValues[i] = 0;
      }
    }
    delete [] m_pValues;
    m_pValues = 0;
  }
}

void BrukerParameter::SetMultiValueMode(bool on) {
  if (!m_pValues) AllocateValuesArray();
  if (!on) {
    m_CurrentValueNum++;
  }
  m_bMultiValueModeOn = on; 
}

void BrukerParameter::PrintParameter(std::ostream &s)
{
  switch (GetParameterType()) {

  case PARAM_TYPE_VIS:
    s << "$$ @vis= ";
    PrintValues(s);
    s << std::endl;
    break;
  case PARAM_TYPE_INFO:
    s << "##";
    PrintValues(s);
    s << std::endl;
    break;

  case PARAM_TYPE_PARAM:
    s << "##$" << GetName() << "=";
    if (m_dimensions.size() != 0) {
      s << "( ";
      for (unsigned int i = 0; i < m_dimensions.size(); i++) {
	s << m_dimensions[i];
	if (i < (m_dimensions.size()-1)) s << ", ";
      }
      s << " )" << std::endl;
    }
    PrintValues(s);
    s << std::endl;
    break;

  }
}

void BrukerParameter::PrintValues(std::ostream &s)
{
  const int MAX_LINE_LENGTH = 79;
  std::stringstream ss;
  for (int i = 0; i < m_NumValues; i++) {
    m_pValues[i]->PrintValue(ss);
    if (i < (m_NumValues -1)) {
      ss << " \n";
    } else {
      ss << "\n";
    }
  }
  
  if (GetParameterType() == PARAM_TYPE_PARAM) {
    /* Now we have all the values in a string, let's organize linebreaks */
    std::string value_string = ss.str();
    std::string cur_word;
    std::stringstream ss_out,ss_tmp;
    char c;
    int current_line_length = 0;
    int current_word_length = 0;
    
    for (unsigned int i = 0; i < value_string.length(); i++) {
      c = value_string.at(i);
      if (c != '\n') { //Add to current word
	ss_tmp << c;
	current_word_length++;
      } else { //Word must have ended
	if (current_word_length) {
	  cur_word = ss_tmp.str();
	  ss_tmp.str(std::string(""));
	  if ((current_word_length + current_line_length) < MAX_LINE_LENGTH) {
	    ss_out << cur_word;
	    current_line_length += current_word_length;
	  } else {
	    ss_out << std::endl << cur_word;
	    current_line_length = current_word_length;
	  }
	  current_word_length = 0;
	}
      }
      
    }
    s << ss_out.str();
  } else {
    s <<  m_pValues[0]->GetStringValue();
  }

}

BrukerParameterValue* BrukerParameter::GetValue(unsigned int val_no)
{
  if (static_cast<int>(val_no) >= m_NumValues) return 0;

  return m_pValues[val_no];
}

void BrukerParameter::AddParameterBefore(BrukerParameter* p)
{
  if (m_pPreviousParameter) {
    m_pPreviousParameter->m_pNextParameter = p;
    p->m_pPreviousParameter = m_pPreviousParameter;
    p->m_pNextParameter = this;
    m_pPreviousParameter = p;
  } else {
    m_pPreviousParameter = p;
    p->m_pNextParameter = this;
  }
}

void BrukerParameter::AddParameterAfter(BrukerParameter* p)
{
  //std::cout << "Adding parameter: " << p->GetName() << std::endl;
  if (m_pNextParameter) {
    p->m_pPreviousParameter = this;
    p->m_pNextParameter = m_pNextParameter;
    m_pNextParameter->m_pPreviousParameter = p;
    m_pNextParameter = p;
  } else {
    m_pNextParameter = p;
    p->m_pPreviousParameter = this;
  }

}

void BrukerParameter::DeleteParameterFromChain()
{
  if (m_pNextParameter) {
    m_pNextParameter->m_pPreviousParameter = m_pPreviousParameter;
  }
  if (m_pPreviousParameter) {
    m_pPreviousParameter->m_pNextParameter = m_pNextParameter;
  }
  m_pNextParameter = 0;
  m_pPreviousParameter = 0;
}


BrukerParameterValue::BrukerParameterValue()
  : m_pNextValue(0),
    m_pPreviousValue(0),
    m_FloatValue(0.0),
    m_IntValue(0),
    m_StringValue(""),
    m_ValueType(PARAM_VALUE_TYPE_NONE)
{

}


BrukerParameterValue::~BrukerParameterValue()
{

  /* Delete all linked values */
  if (m_pPreviousValue) {
    delete m_pPreviousValue;
    m_pPreviousValue = 0;
  }
  if (m_pNextValue) {
    delete m_pNextValue;
    m_pNextValue = 0;
  }
}

void BrukerParameterValue::SetStringValue(std::string v, bool append_if_set = false)
{
  if (m_ValueType == PARAM_VALUE_TYPE_NONE || !append_if_set) {
    m_ValueType = PARAM_VALUE_TYPE_STRING;
    m_StringValue = v;
    m_IntValue = m_StringValue.size();
    m_FloatValue = m_StringValue.size();
  } else {
    BrukerParameterValue* last = GetLastParameterValueInChain();
    last->m_pNextValue = new BrukerParameterValue();
    last->m_pNextValue->SetStringValue(v);
    last->m_pNextValue->m_pPreviousValue = last;
  }
}

void BrukerParameterValue::SetLabelValue(std::string v, bool append_if_set = false)
{
  if (m_ValueType == PARAM_VALUE_TYPE_NONE || !append_if_set) {
    m_ValueType = PARAM_VALUE_TYPE_LABEL;
    m_StringValue = v;
    m_IntValue = m_StringValue.size();
    m_FloatValue = m_StringValue.size();
  } else {
    BrukerParameterValue* last = GetLastParameterValueInChain();
    last->m_pNextValue = new BrukerParameterValue();
    last->m_pNextValue->SetLabelValue(v);
    last->m_pNextValue->m_pPreviousValue = last;
  }
}

void BrukerParameterValue::SetIntValue(std::string v, bool append_if_set = false)
{
  if (m_ValueType == PARAM_VALUE_TYPE_NONE || !append_if_set) {
    m_ValueType = PARAM_VALUE_TYPE_INT;
    m_StringValue = v;
    m_IntValue = atoi(v.c_str());
    m_FloatValue = static_cast<double>(m_IntValue);
  } else {
    BrukerParameterValue* last = GetLastParameterValueInChain();
    last->m_pNextValue = new BrukerParameterValue();
    last->m_pNextValue->SetIntValue(v);
    last->m_pNextValue->m_pPreviousValue = last;
  }

}

void BrukerParameterValue::SetFloatValue(std::string v, bool append_if_set = false)
{
  if (m_ValueType == PARAM_VALUE_TYPE_NONE || !append_if_set) {
    m_ValueType = PARAM_VALUE_TYPE_FLOAT;
    m_StringValue = v;
    m_FloatValue = atof(v.c_str());
    m_IntValue = static_cast<int>(floor((m_FloatValue+0.5)));
  } else {
    BrukerParameterValue* last = GetLastParameterValueInChain();
    last->m_pNextValue = new BrukerParameterValue();
    last->m_pNextValue->SetFloatValue(v);
    last->m_pNextValue->m_pPreviousValue = last;
  }
}


BrukerParameterValue* BrukerParameterValue::GetLastParameterValueInChain()
{
  BrukerParameterValue* ret = this;
  while(ret->m_pNextValue) ret = ret->m_pNextValue;
  return ret;
}

BrukerParameterValue* BrukerParameterValue::GetFirstParameterValueInChain()
{
  BrukerParameterValue* ret = this;
  while(ret->m_pPreviousValue) ret = ret->m_pPreviousValue;
  return ret;
}

BrukerParameterValue* BrukerParameterValue::GetParameterValueInChain(unsigned int index) 
{
  BrukerParameterValue* ret = 0;
  
  if (index < GetNumberOfParameterValuesInChain()) {
    ret = GetFirstParameterValueInChain();
    for (unsigned int i = 0; i < index; i++) {
      ret = ret->m_pNextValue;
    } 
  }
  return ret;
}

unsigned int BrukerParameterValue::GetNumberOfParameterValuesInChain()
{
  unsigned int return_val = 0;
  BrukerParameterValue* first = GetFirstParameterValueInChain();
  while (first) {
    return_val++;
    first = first->m_pNextValue;
  }
  return return_val;
}


void BrukerParameterValue::PrintValue(std::ostream &s) 
{
  //Is this a multi value field
  if (m_pNextValue) {
    s << "\n(";
  }
  BrukerParameterValue* first = this;
  while (first) {
    if (first->m_ValueType == PARAM_VALUE_TYPE_STRING) {
      s << "\n<";
    }
    s << first->GetStringValue();
    if (first->m_ValueType == PARAM_VALUE_TYPE_STRING) {
      s << ">\n";
    }
     if (first->m_pNextValue) {
       s << ",\n \n";
    }
    first = first->m_pNextValue;
  }
  if (m_pNextValue) {
    s << ")\n";
  }
  
}



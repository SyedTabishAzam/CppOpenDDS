/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"
#include "CommanderTypeSupportC.h"
#include "CommanderTypeSupportImpl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
using namespace std;
void
DataReaderListenerImpl::on_requested_deadline_missed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_requested_incompatible_qos(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_rejected(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_liveliness_changed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
  Commander::CommandDataReader_var reader_i =
    Commander::CommandDataReader::_narrow(reader);

  if (!reader_i) {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" _narrow failed!\n")));
    ACE_OS::exit(-1);
  }

  Commander::Command message;
  DDS::SampleInfo info;

  DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

  if (error == DDS::RETCODE_OK) {
    std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
    std::cout << "SampleInfo.instance_state = " << info.instance_state << std::endl;

	string line;
	fstream myfile;
	myfile.open("S:\\Presagis\\Suite16\\STAGE\\GUIPlugin\\entitymission.missionactive", ios::out);

    if (info.valid_data) {
  /*    std::cout << "         id = "         << message.id   << std::endl
                << "         time       = " << message.time.in()    << std::endl
                << "         count      = " << message.count        << std::endl
                << "         command       = " << message.key.in()    << std::endl
                << "         value       = " << message.value.in()    << std::endl;*/
		
		string s1 = "CONNECTION_ESTABLISHED";
		string s2 = message.key.in();
		if (s2.find(s1) != std::string::npos)
		{
			std::cout << "Connection Established" << std::endl;
		}
		else
		{
			myfile << "TRUE " << message.key.in() << " " << message.value.in() << std::endl;
		}

    }

  } else {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" take_next_sample failed!\n")));
  }
}

void
DataReaderListenerImpl::on_subscription_matched(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_lost(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleLostStatus& /*status*/)
{
}

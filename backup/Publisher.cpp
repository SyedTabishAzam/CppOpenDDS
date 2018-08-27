/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <ace/Log_Msg.h>

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>

#include "dds/DCPS/StaticIncludes.h"

#include "CommanderTypeSupportImpl.h"

const int NUM_SECONDS = 3;

using namespace std;

string writePath = "commands.txt";
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{

  try {
	  int fps = 0;
    // Initialize DomainParticipantFactory
    DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);

    // Create DomainParticipant
    DDS::DomainParticipant_var participant = dpf->create_participant(42,PARTICIPANT_QOS_DEFAULT,0,OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!participant)
    {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_participant failed!\n")),
                       -1);
    }

    // Register TypeSupport (Messenger::Message)
    Commander::CommandTypeSupport_var ts = new Commander::CommandTypeSupportImpl;

    if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" register_type failed!\n")),
                       -1);
    }

    // Create Topic 
    CORBA::String_var type_name = ts->get_type_name();
    DDS::Topic_var topic =
      participant->create_topic("Commands list",
                                type_name,
                                TOPIC_QOS_DEFAULT,
                                0,
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!topic) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_topic failed!\n")),
                       -1);
    }

    // Create Publisher
    DDS::Publisher_var publisher =
      participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                    0,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!publisher) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_publisher failed!\n")),
                       -1);
    }

    // Create DataWriter
    DDS::DataWriter_var writer =
      publisher->create_datawriter(topic,
                                   DATAWRITER_QOS_DEFAULT,
                                   0,
                                   OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!writer) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_datawriter failed!\n")),
                       -1);
    }

    Commander::CommandDataWriter_var message_writer = Commander::CommandDataWriter::_narrow(writer);

    if (!message_writer) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" _narrow failed!\n")),
                       -1);
    }
	printf("PUBLISHER STARTED\n");
    // Block until Subscriber is available
    DDS::StatusCondition_var condition = writer->get_statuscondition();
    condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);

    DDS::WaitSet_var ws = new DDS::WaitSet;
    ws->attach_condition(condition);

    while (true) {
      DDS::PublicationMatchedStatus matches;
      if (writer->get_publication_matched_status(matches) != ::DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" get_publication_matched_status failed!\n")),
                         -1);
      }

      if (matches.current_count >= 1) {
        break;
      }

      DDS::ConditionSeq conditions;
      DDS::Duration_t timeout = { 60, 0 };
      if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" wait failed!\n")),
                         -1);
      }
    }

    ws->detach_condition(condition);


	

	int count = 1;

	double time_counter = 0;

	clock_t this_time = clock();
	clock_t last_time = this_time;
	
	printf("Gran = %ld\n", NUM_SECONDS * CLOCKS_PER_SEC);


	//Run the code after every N seconds
	while (true)
	{
		this_time = clock();

		time_counter += (double)(this_time - last_time);

		last_time = this_time;

		//if N seconds has passed then read file. otherwise continue
		if (time_counter > (double)(NUM_SECONDS * CLOCKS_PER_SEC))
		{
			time_counter -= (double)(NUM_SECONDS * CLOCKS_PER_SEC);
				
			//Read a file
			string line;
			fstream myfile;
			myfile.open(writePath, ios::in);
			
			
			Commander::Command message;
			message.id = 99;

			bool dataRead = true;
			if (myfile.is_open())
			{
				getline(myfile, line);
				if (line != "NEW_COMMANDS")
				{
					cout << line << endl;
					cout << "No new commands found" << endl;
					myfile.close();
					continue;
				}
				//Open log file only if new commands are found
				fstream log;
				log.open("log.txt", ios::out);
				while (getline(myfile, line))
				{
					
					

					std::vector<std::string> v;
					split(line, v, ' ');

					

					// Write samples
					
					const char* time = v.at(0).c_str();
					const char* key = v.at(1).c_str();
					const char* value = v.at(2).c_str();

					message.time = time;
					message.key = key;
					message.value = value;
					message.count = 0;


					DDS::ReturnCode_t error = message_writer->write(message, DDS::HANDLE_NIL);
					++message.count;
					++message.id;

					if (error != DDS::RETCODE_OK) {
						ACE_ERROR((LM_ERROR,
							ACE_TEXT("ERROR: %N:%l: main() -")
							ACE_TEXT(" write returned %d!\n"), error));
					}


					// Wait for samples to be acknowledged
					DDS::Duration_t timeout = { 30, 0 };
					if (message_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK) {
						ACE_ERROR_RETURN((LM_ERROR,
							ACE_TEXT("ERROR: %N:%l: main() -")
							ACE_TEXT(" wait_for_acknowledgments failed!\n")),
							-1);
						dataRead = dataRead && false;
						log << line << ": FAILED" << endl;
					}
					else
					{
						dataRead = dataRead && true;
						//Printing and writing codes to log
						cout << line << endl;
						log << line <<": SUCCESS"<< endl;
					}
					
				}
				//myfile.clear();
				//myfile << "NOCOMMAND";
				myfile.close();
				log.close();
				myfile.open(writePath, std::ofstream::out | std::ofstream::trunc);
				if (dataRead)
				{
					
					myfile << "STATUS SUBSCRIBER_SUCCESS" << endl;
					
					cout << "All commands acknowledged by subscriber" << endl;
				}
				else
				{
					myfile << "STATUS SUBSCRIBER_FAILED" << endl;
					
					cout << "Command acknowledgement failed! Please see the log file" << endl;
				}
				
				myfile.close();

				
			}
			else
			{
				ACE_ERROR_RETURN((LM_ERROR,
					ACE_TEXT("ERROR: %N:%l: main() -")
					ACE_TEXT(" Can't open file!\n")),
					-1);
				break;
			}

			count++;
		}

			
	}

		
	
    

    // Clean-up!
    participant->delete_contained_entities();
    dpf->delete_participant(participant);

    TheServiceParticipant->shutdown();

  } catch (const CORBA::Exception& e) {
    e._tao_print_exception("Exception caught in main():");
    return -1;
  }

  return 0;
}



#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <mraa.hpp>
#include <memory.h>
#include <boost/property_tree/json_parser.hpp>
#include "../lib/restclient-cpp/include/restclient-cpp/restclient.h"
#include "../lib/json-cpp/json/json.h"
#include "../lib/curl/curl.h"
#include <ctime>
#include <string>
using namespace std;
char access_code[5];
int i = 0, number;
mraa_gpio_context info;
bool flag = 0;

void speak(string status) {
//	system(("espeak -w speech.wav -a 120 -g 8 -k 1 -p 45 -s 190 '" + status + "' ; gst-launch-1.0 filesrc location= /home/root/speech.wav ! wavparse ! pulsesink").c_str());
	system(("espeak -w speech.wav -a 50 -g 8 -k 1 -p 45 -s 190 '" + status + "' ; gst-launch-1.0 filesrc location= /home/root/speech.wav ! wavparse ! pulsesink").c_str());
}

void weather() {
	string State = "Penang";

	RestClient::response r = RestClient::get(("http://api.openweathermap.org/data/2.5/weather?q=" + State + "&appid=a92784b8dba949baa3d5c2f199ea8d12").c_str());

	string str = r.body; // need escape the quotes

	for (int i = 0; i < int(str.length()); i++){
        if ((int(str[i]) == 91) || (int(str[i]) == 93))  str[i] = ' ';
    }

    boost::property_tree::ptree parser;
    stringstream ss(str);
    boost::property_tree::json_parser::read_json(ss, parser);

    //Get "Weather Status"
    boost::property_tree::ptree user_array = parser.get_child("weather");
    string weather_status = user_array.get<string>("description");

    speak(("Today''s weather is " + weather_status).c_str());
}

int security() {
	RestClient::response r;
	stringstream query;
    srand(time(NULL));
    speak("Generating Door Access Code!");
	number = rand() % 9000 + 1000;
	snprintf(access_code, 5, "%d", number);
	i = 0;
	while (i <= 3){
		if (access_code[i] == '0') {
			number = rand() % 9000 + 1000;
			snprintf(access_code, 5, "%d", number);
			i = 0;
		} else i++;
	}

	speak("Preparing SMS to be sent!");

/*	query.str("");
    query << "To=" << "%2B60125478256" << "&";
	query << "From=" << "%2B19802382273" << "&";
	query << "Body=" << "URGENT! Your elderly needs your help! Your 4 digit door access code is " + to_string(number) + ". Please come to the house as soon as possible!";
	r = RestClient::post("https://ACee064cb9668059ccd76f7ae8ba01b1e8:2ec12a092d7ae4d52bb1ccbd6a596d2f@api.twilio.com/2010-04-01/Accounts/ACee064cb9668059ccd76f7ae8ba01b1e8/Messages.json","application/x-www-form-urlencoded",query.str());
*/
	system("sleep 3");
	query.str("");
    query << "To=" << "%2B60189888256" << "&";
	query << "From=" << "%2B19802382273" << "&";
	query << "Body=" << "URGENT! Your elderly needs your help! Your 4 digit door access code is " + to_string(number) + ". Please come to the house as soon as possible!";
	r = RestClient::post("https://ACee064cb9668059ccd76f7ae8ba01b1e8:2ec12a092d7ae4d52bb1ccbd6a596d2f@api.twilio.com/2010-04-01/Accounts/ACee064cb9668059ccd76f7ae8ba01b1e8/Messages.json","application/x-www-form-urlencoded",query.str());

	freopen ("/media/sdcard/password.txt","w", stdout);
	printf(access_code);
	fclose(stdout);
	speak("Relatives have been notified!");

	return number;
}

void greet(){
	int hour;

	time_t now = time(0);
	tm *ltm = localtime(&now);

	hour = ltm->tm_hour;
	cout << "Hour: " + to_string(hour) << endl;
	if (hour >= 0 && hour <= 11) speak("Good morning sir!");
	if (hour >= 12 && hour <= 16) speak("Good afternoon sir!");
	if (hour >= 17 && hour <= 23) speak("Good evening sir!");
}

void tips(int condition_1, int condition_2){
  if (condition_1 == 0 && condition_2 == 0) speak("Congratulations! You are in good health. Please maintain it!");
  if ((condition_1 == 0 || condition_1 == 1) && condition_2 == 1) speak("Please have some exercises or work out!");
  if ((condition_1 == 0 || condition_1 == 1) && condition_2 == 2) speak("Please drink more water!");
  if (condition_1 == 1 && condition_2 == 0) speak("Please calm yourself down by listening to musics or watch TV!");
  if (condition_1 == 2) speak("Your heart rate is too high! Please consult doctor for immediate consultation");
}

int retrieveWeb_HeartBeat(){
	bool condition = 0;
	stringstream query;
	i = 1;
	RestClient::response r = RestClient::get("https://thingspeak.com/channels/271172/field/1.json?results=20");

	string str = (r.body + "                                                                                                                                                        ").c_str();
	int string_length = int(str.length());
	int count = 49;
	for (int i = 0; i < string_length; i++){
        if ((int(str[i]) == 91) || (int(str[i]) == 93))  str[i] = ' ';
        if ((int(str[i]) == 102) && (int(str[i+1] == 101)) && (int(str[i+2]) == 101) && (int(str[i+3]) == 100) && (int(str[i+4]) == 115)){
        	str[i+4] = '0';
        }
        if ((int(str[i]) == 125) && (int(str[i+1] == 44)) && (int(str[i+2]) == 123)) {
        	for (int move_value = 0; move_value < 8; move_value++){
        		for (int j = string_length; j > (i+2); j--){
        			str[j] = str[j-1];
        		}
        	}
        	if (count <= 58 && condition == 0) {
        		str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 'd';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
        		if (count == 58) {
        			count = 48;
        			condition = 1;
        		}
        	}
        	if (count <= 57 && condition == 1) {
        		str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 't';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
        	}
        	count++;
        }
    }
	cout << str << endl;
	boost::property_tree::ptree parser;
    stringstream ss(str);
    boost::property_tree::json_parser::read_json(ss, parser);

    //Get Field 1 Value
    int total = 0;
    float average;
	for (i = 0; i < 10; i++){
		boost::property_tree::ptree user_array = parser.get_child(("feed"+to_string(i)).c_str());
		string value = user_array.get<string>("field1");
		if (value == "" || value == "null") value = "0";
		total += stoi(value);
	}
	for (i = 0; i < 10; i++){
		boost::property_tree::ptree user_array = parser.get_child(("feet"+to_string(i)).c_str());
		string value = user_array.get<string>("field1");
		if (value == "" || value == "null") value = "0";
		total += stoi(value);
	}
	cout << "Total: " + to_string(total) << endl;
	average = (float) total / 20;
	cout << "Average: " + to_string(average) << endl;
	if (average > 50 && average < 77) {
		speak ("Your heart rate is normal!");
		return 0;
	}
	if (average < 50 || (average >= 77 && average < 84)) {
		speak ("Your heart rate is abnormal!");
		return 1;
	}
	if (average > 84) {
		speak ("Your heart rate is too high!");
		return 2;
	}
}

int retrieveWeb_Temperature(){
	bool condition = 0;
	stringstream query;
	i = 1;
	RestClient::response r = RestClient::get("https://thingspeak.com/channels/271172/field/2.json?results=20");

	string str = (r.body + "                                                                                                                                                        ").c_str();
	int string_length = int(str.length());
	int count = 49;
	for (int i = 0; i < string_length; i++){
        if ((int(str[i]) == 91) || (int(str[i]) == 93))  str[i] = ' ';
        if ((int(str[i]) == 102) && (int(str[i+1] == 101)) && (int(str[i+2]) == 101) && (int(str[i+3]) == 100) && (int(str[i+4]) == 115)){
        	str[i+4] = '0';
        }
        if ((int(str[i]) == 125) && (int(str[i+1] == 44)) && (int(str[i+2]) == 123)) {
        	for (int move_value = 0; move_value < 8; move_value++){
        		for (int j = string_length; j > (i+2); j--){
        			str[j] = str[j-1];
        		}
        	}
        	if (count <= 58 && condition == 0) {
        		str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 'd';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
        		if (count == 58) {
        			count = 48;
        			condition = 1;
        		}
        	}
        	if (count <= 57 && condition == 1) {
        		str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 't';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
        	}
        	count++;
        }
    }
	cout << str << endl;
	boost::property_tree::ptree parser;
    stringstream ss(str);
    boost::property_tree::json_parser::read_json(ss, parser);

    //Get Field 2 Value
    int total = 0;
    float average;
	for (i = 0; i < 10; i++){
		boost::property_tree::ptree user_array = parser.get_child(("feed"+to_string(i)).c_str());
		string value = user_array.get<string>("field2");
		if (value == "" || value == "null") value = "0";
		total += stoi(value);
	}
	for (i = 0; i < 10; i++){
		boost::property_tree::ptree user_array = parser.get_child(("feet"+to_string(i)).c_str());
		string value = user_array.get<string>("field2");
		if (value == "" || value == "null") value = "0";
		total += stoi(value);
	}
	cout << "Total: " + to_string(total) << endl;
	average = (float) total / 20;
	cout << "Average: " + to_string(average) << endl;
	if (average >= 36 && average <= 37) {
		speak ("Your body temperature is normal!");
		return 0;
	}
	if (average < 36) {
		speak ("Your body temperature is too low!");
		return 1;
	}
	if (average > 37) {
		speak ("Your body temperature is too high!");
		return 2;
	}
}

int retrieveWeb_Acceleration(){
	bool condition = 0;
	RestClient::response r;
	string str;
	int remark_value, loop = 0, total = 0, state = 0, string_length = 0, count;
	stringstream query;
	i = 1;
	speak("Entering fall detection mode");
	if (mraa_gpio_read(info) == 0) flag = 1;
	else if (flag == 0) {
		r = RestClient::get("https://thingspeak.com/channels/271172/field/3.json?results=20");

		str = (r.body + "                                                                                                                                                        ").c_str();
		int string_length = int(str.length());
		count = 49;

		for (int i = 0; i < string_length; i++){
			if (mraa_gpio_read(info) == 0) flag = 1;
			else if (flag == 0) {
				if ((int(str[i]) == 91) || (int(str[i]) == 93))  str[i] = ' ';
				if ((int(str[i]) == 102) && (int(str[i+1] == 101)) && (int(str[i+2]) == 101) && (int(str[i+3]) == 100) && (int(str[i+4]) == 115)){
					str[i+4] = '0';
				}
				if ((int(str[i]) == 125) && (int(str[i+1] == 44)) && (int(str[i+2]) == 123)) {
					for (int move_value = 0; move_value < 8; move_value++){
						for (int j = string_length; j > (i+2); j--){
							if (mraa_gpio_read(info) == 0) flag = 1;
							else if (flag == 0) str[j] = str[j-1];
						}
					}
					if (count <= 58 && condition == 0) {
						str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 'd';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
						if (count == 58) {
							count = 48;
							condition = 1;
						}
					}
					if (count <= 57 && condition == 1) {
						str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 't';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
					}
					count++;
				}
			}
		}
	}
	if (mraa_gpio_read(info) == 0) flag = 1;
	else if (flag == 0) {
		boost::property_tree::ptree parser;
		stringstream ss(str);
		boost::property_tree::json_parser::read_json(ss, parser);
		cout << str << endl;

		for (i = 9; i >= 0; i--){
			if (mraa_gpio_read(info) == 0) flag = 1;
			else if (flag == 0) {
				if (state == 0){
					boost::property_tree::ptree user_array = parser.get_child(("feet"+to_string(i)).c_str());
					string value = user_array.get<string>("field3");
					if (value == "" || value == "null") value = "0";
					total = stoi(value);
				}
				if ((total > 20000 || (total*-1 > 20000)) && state == 1){
					system("sleep 5");
					r = RestClient::get("https://thingspeak.com/channels/271172/field/3.json?results=3");

					str = (r.body + "                                                                                                                                                        ").c_str();
					string_length = int(str.length());
					count = 49;
					for (int i = 0; i < string_length; i++){
						if (mraa_gpio_read(info) == 0) flag = 1;
						else if (flag == 0) {
							if ((int(str[i]) == 91) || (int(str[i]) == 93))  str[i] = ' ';
							if ((int(str[i]) == 102) && (int(str[i+1] == 101)) && (int(str[i+2]) == 101) && (int(str[i+3]) == 100) && (int(str[i+4]) == 115)){
								str[i+4] = '0';
							}
							if ((int(str[i]) == 125) && (int(str[i+1] == 44)) && (int(str[i+2]) == 123)) {
								for (int move_value = 0; move_value < 8; move_value++){
									for (int j = string_length; j > (i+2); j--){
										if (mraa_gpio_read(info) == 0) flag = 1;
										else if (flag == 0) str[j] = str[j-1];
									}
								}
								if (count <= 58 && condition == 0) {
									str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 'd';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
									if (count == 58) {
										count = 48;
										condition = 1;
									}
								}
								if (count <= 57 && condition == 1) {
									str[i+2] = '"';	str[i+3] = 'f';	str[i+4] = 'e';	str[i+5] = 'e';	str[i+6] = 't';	str[i+7] = count;	str[i+8] = '"';	str[i+9] = ':';
								}
								count++;
							}
						}
					}

					if (mraa_gpio_read(info) == 0) flag = 1;
					else if (flag == 0) {
						boost::property_tree::ptree parser;
						stringstream ss(str);
						boost::property_tree::json_parser::read_json(ss, parser);
						for (i = 9; i >= 0; i--){
							if (mraa_gpio_read(info) == 0) flag = 1;
							else if (flag == 0) {
								boost::property_tree::ptree user_array = parser.get_child(("feet"+to_string(i)).c_str());
								string value = user_array.get<string>("field3");
								if (value == "" || value == "null") value = "0";
								total = stoi(value);
								if	(loop == 0) {
									speak("Checking mode");
									remark_value = total;
									loop++;
								} else if (total >= (remark_value - 50) && total <= (remark_value + 50) && loop > 0) loop++;
								if (loop >= 1) {
									speak("Are you ok? Please press the button if you are ok!");
									for (int time = 0; time < 60; time++){
										if (mraa_gpio_read(info) == 0) {
											security();
											return 1;
										}
										system("sleep 1");
									}
									return 0;
								}
							}
						}
					}
				}
			}
		}
	}
	speak("Existing fall detection mode");
	return 1;
}

int main(){
	mraa_init();
	int condition_1 = 0, condition_2 = 0, condition_3 = 0;

	info	= mraa_gpio_init(0);
	mraa_gpio_dir(info, MRAA_GPIO_IN);

	system("rdate wwv.nist.gov");
    while(1){
    	if (flag == 0) condition_3 = retrieveWeb_Acceleration();
    	else {
    		while (mraa_gpio_read(info) == 0){
    			greet();
    			condition_1 = retrieveWeb_HeartBeat();
    			condition_2 = retrieveWeb_Temperature();
    			tips(condition_1, condition_2);
    			flag = 0;
    			system("sleep 3");
//    			security();
    			break;
    		}
    	}
    }
	return MRAA_SUCCESS;
}

/*
*********************Emergency Notification*********************
      if (emergency_signal == 1){
      	  speak("Are you ok, sir?");
      	  system("sleep 30");
      	  if (emergency_signal == 1){
	  	  	  speak("Preparing door access code to be sent.");
	  	  	  security();
	  	  	  speak("Relatives have been notified!");
			  break;
		  }
		}
*/

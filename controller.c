#include <stdio.h>  
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>  
#include <stdint.h>  
#include <telldus-core.h>
#include <unistd.h>
#include <getopt.h>

bool exec_command(const char* command, char buf[], int buf_size) {
  memset(buf, '\0', buf_size);  
  FILE* fp_read = popen(command, "r");
  if (fp_read != NULL) {
    while(fgets(buf, buf_size-1, fp_read) != NULL) {}
    pclose(fp_read);
    return true;  
  }
  return false;
}

bool readtemp() {
  const char* command = "temp=$(cat /sys/bus/w1/devices/28-03146596e8ff/w1_slave | grep  -E -o \".{0,0}t=.{0,5}\" | cut -c 3-)&& echo \"scale=2;$temp/1000\"|bc";
  char buf[64];
  bool success = exec_command(command, buf, 64);
  
  printf("%s", buf);
  return success;
}

bool status() {
  const char* command = "uptime | cut -d',' -f1 | sed -e 's/^[ \t]*//'";
  char uptime_buf[128];  
  exec_command(command, uptime_buf, 128);
  printf("Time: %s", uptime_buf);
  
  int number_of_devices = tdGetNumberOfDevices();
  for (int i = 0; i < number_of_devices; i++) {
    int id = tdGetDeviceId(i);
    char *name = tdGetName(id);
    int state = tdLastSentCommand(id, TELLSTICK_TURNON | TELLSTICK_TURNOFF);

    printf("Device %d: %s - %s\n", id, name, (state == TELLSTICK_TURNON)?"ON":"OFF");
    tdReleaseString(name);
  }
  return true;
}

bool switch_one(int device_id, bool on) {
  int status;
  if(on) {
    status = tdTurnOn(device_id);
  } else {
    status = tdTurnOff(device_id);
  }
  return status == 0;
}
 
bool switch_all(bool on) {
  int number_of_devices = tdGetNumberOfDevices();
  bool ret = true;
  for (int i = 0; i < number_of_devices; i++) {
    int id = tdGetDeviceId(i);
    ret &= switch_one(id, on);
  }
  return ret;
}

bool do_switch_command(bool on) {
  if(strcmp("all", optarg) == 0) {
    return switch_all(on);
  } else {
    int device_id = atoi(optarg);
    return switch_one(device_id, on);
  }
}

int main(int argc, char* argv[]) {  
  // init tellstick
  tdInit();

  int num_options = 4;
  static struct option long_options[] = {
    {"status", no_argument, 0, 0},
    {"readtemp", no_argument, 0, 1},
    {"on", required_argument, 0, 2},
    {"off", required_argument, 0, 3},
    {0, 0, 0, 0}
  };
    
  int c;
  bool success = false;

  while(1) {    
    int option_index = 0;
    c = getopt_long_only(argc, argv, "", long_options, &option_index);
    if (c == -1) {  
      break;
    }
    
    switch(c) {
      // status
    case 0: 
      success = status();
      break;
      // readtemp
    case 1:
      success = readtemp();
      break;
      // on
    case 2:
      if(optarg) {
	success = do_switch_command(true);
      }
      break;
      // off
    case 3:
      if(optarg) {
	success = do_switch_command(false);
      }      
      break;
    }
  }

  if(!success) {
    printf("Available options:\n");      
    for(int i = 0; i < num_options; i++) {
      printf("\t%s\n", long_options[i].name);
    }
  }
  
}

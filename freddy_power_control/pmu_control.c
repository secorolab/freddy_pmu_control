
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "ethercat.h"
#include "ethercatbase.h"
#include "ethercatcoe.h"
#include "ethercatconfig.h"
#include "ethercatdc.h"
#include "ethercatmain.h"
#include "ethercatprint.h"
#include "ethercattype.h"
#include "nicdrv.h"

char IOmap[4096];

typedef struct PACKED
{
  uint16_t status;
  uint64_t time_stamp;
  float current;
  float voltage;
  float power;
  float param1;
  uint32_t param2;
} txpdo1_t;

typedef struct PACKED
{
  uint16_t shutdown;
  uint32_t command;
} rxpdo1_t;

int main(int argc, char *argv[])
{

  ec_slavet ecx_slave[EC_MAXSLAVE];
  int ecx_slavecount;
  ec_groupt ec_group[EC_MAXGROUP];
  uint8 esibuf[EC_MAXEEPBUF];
  uint32 esimap[EC_MAXEEPBITMAP];
  ec_eringt ec_elist;
  ec_idxstackT ec_idxstack;

  ec_SMcommtypet ec_SMcommtype;
  ec_PDOassignt ec_PDOassign;
  ec_PDOdesct ec_PDOdesc;
  ec_eepromSMt ec_SM;
  ec_eepromFMMUt ec_FMMU;
  boolean EcatError;
  int64 ec_DCtime;
  ecx_portt ecx_port;
  ecx_contextt ecx_context;
  char IOmap[4096];

  ecx_context.port = &ecx_port;
  ecx_context.slavelist = &ecx_slave[0];
  ecx_context.slavecount = &ecx_slavecount;
  ecx_context.maxslave = EC_MAXSLAVE;
  ecx_context.grouplist = &ec_group[0];
  ecx_context.maxgroup = EC_MAXGROUP;
  ecx_context.esibuf = &esibuf[0];
  ecx_context.esimap = &esimap[0];
  ecx_context.esislave = 0;
  ecx_context.elist = &ec_elist;
  ecx_context.idxstack = &ec_idxstack;

  ecx_context.ecaterror = &EcatError;
  ecx_context.DCtime = &ec_DCtime;
  ecx_context.SMcommtype = &ec_SMcommtype;
  ecx_context.PDOassign = &ec_PDOassign;
  ecx_context.PDOdesc = &ec_PDOdesc;
  ecx_context.eepSM = &ec_SM;
  ecx_context.eepFMMU = &ec_FMMU;
  ecx_context.manualstatechange = 0; // should be 0

  if (!ecx_init(&ecx_context, "enp2s0"))
  {
    printf("Failed to initialize EtherCAT\n");
    return 0;
  }

  if (!ecx_config_init(&ecx_context, TRUE))
  {
    printf("NO SLAVES!\n");
    return 0;
  }
  ecx_config_map_group(&ecx_context, IOmap, 0); // PDO - process data object

  printf("slave count: %i\n", ecx_slavecount);
  printf("first slave name: %s\n", ecx_slave[1].name);

  /**
   * @brief Reading all slave names w.r.t their no.
   *
   */
  for (int i = 1; i <= ecx_slavecount; i++)
  {
    printf("slave [%i] has name [%s]\n", i, ecx_slave[i].name);
  }

  /**
   * @brief waiting for all slaves to reach SAFE_OP state
   *
   */
  ecx_statecheck(&ecx_context, 0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);

  if (ecx_slave[0].state != EC_STATE_SAFE_OP) // with index 0, we are checking the state of all slaves
  {
    printf("EtherCAT slaves have not reached safe operational state\n");
    ecx_readstate(&ecx_context);

    /**
     * @brief if not all slaves operational, find out which one
     *
     */
    for (int i = 1; i <= ecx_slavecount; i++)
    {
      if (ecx_slave[i].state != EC_STATE_SAFE_OP)
      {
        printf("Slave %i State= %i\n", i, ecx_slave[i].state);
      }
    }
    return 0;
  }

  /**
   * @brief receiving response from slaves
   *
   */
  ecx_send_processdata(&ecx_context);
  ecx_receive_processdata(&ecx_context, EC_TIMEOUTRET);

  /**
   * @brief checking if the slaves have reached operational state
   *
   */
  ecx_statecheck(&ecx_context, 0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE);

  txpdo1_t *a = (txpdo1_t *)ecx_slave[1].inputs;

  printf("voltage: %f\n", a->voltage);
  printf("current: %f\n", a->current);
  printf("power: %f\n", a->power);
  printf("status: %d\n", a->status);
  printf("time_stamp: %ld\n", a->time_stamp);

  rxpdo1_t msg;
  // msg.command = 0b00100100; // base off; torso on; // 36
  // msg.command = 0b00000100; // base off; torso off; // 4
  // msg.command = 0b00100111; // base on; torso on; // 39
  msg.command = 0b00100000; // base on (default); torso on; // 32
  // msg.command = 0b00000111; // base on; torso off; // 7

  // msg.shutdown = 0b01000001; // shutdown pmu with 2 seconds delay; // 65
  // msg.shutdown = 0b00000000; // donot shutdown pmu; // 0

  rxpdo1_t *ecData = (rxpdo1_t *)ecx_slave[1].outputs;
  *ecData = msg;

  printf("setting 'command': %d\n", ecData->command);
  printf("setting 'shutdown': %d\n", ecData->shutdown);

  ecx_send_processdata(&ecx_context);

  /**
   * @brief setting state to operational
   *
   */
  ecx_slave[0].state = EC_STATE_OPERATIONAL;

  /**
   * @brief receiving response from slaves
   *
   */
  ecx_receive_processdata(&ecx_context, EC_TIMEOUTRET);

  ecx_writestate(&ecx_context, 0);

  return (0);
}

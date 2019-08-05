/********************************************************************************
 * @file    mylib/s4408736_cli_APCShell.h
 * @author  Kevin Bazurto – 44087368
 * @date    20/05/2018
 * @brief   Handles CLI commands for project 2
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_cli_apcshell_init(void) - initialize CLI
 ******************************************************************************
 */

#ifndef S4408736_CLI_APCSHELL_H_
#define S4408736_CLI_APCSHELL_H_

/* Private define ------------------------------------------------------------*/
//Commands registered in CLI

CLI_Command_Definition_t xGetSys;
CLI_Command_Definition_t xSetChan;
CLI_Command_Definition_t xGetChan;
CLI_Command_Definition_t xSetTxAddr;
CLI_Command_Definition_t xSetRxAddr;
CLI_Command_Definition_t xGetRxAddr;
CLI_Command_Definition_t xGetTxAddr;
CLI_Command_Definition_t xJoin;
CLI_Command_Definition_t xMove;
CLI_Command_Definition_t xPen;
CLI_Command_Definition_t xSetHeight;
CLI_Command_Definition_t xOrigin;
CLI_Command_Definition_t xLine;
CLI_Command_Definition_t xSquare;
CLI_Command_Definition_t xBline;

//define functions to access internal functions
extern void s4408736_cli_apcshell_init(void);

#endif /* S4408736_CLI_APCSHELL_H_ */


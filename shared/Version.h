/* MerDeGlace's version
 *
 * Copyright 2022 Laurent Faillie
 *
 * 		MerDeGlace is covered by
 *		Creative Commons Attribution-NonCommercial 3.0 License
 *      (http://creativecommons.org/licenses/by-nc/3.0/) 
 *      Consequently, you're free to use if for personal or non-profit usage,
 *      professional or commercial usage REQUIRES a commercial licence.
 *  
 *      MerDeGlace is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	04/12/2022 - LF - Start of development
 *	20/12/2022 - LF - v0.4 - End of POC
 *	24/12/2022 - LF - v0.5 - Separate daemon and CLI
 *	30/12/2022 - LF - v0.6 - Handle accept
 *	06/01/2023 - LF - v0.7 - Add md5's checksum
 *	07/01/2023 - LF - v0.8 - Report for duplicate
 *	09/01/2023 - LF - v0.9 - Add alternative root
 *	26/01/2023 - LF - v0.10 - Add return code to MdG
 *	27/01/2023 - LF - v0.11 - Clean up deamon's fatal error
 *	04/02/2023 - LF - v1.00 - Promote to stable version
 */

#define VERSION 1.00
#define COPYRIGHT "Mer de Glace (c) L.Faillie 2022-23"

/* Default configuration file */
#define DEFAULT_CONFIGURATION_FILE "/usr/local/etc/MerDeGlace.conf"

#pragma once

/**
 * @brief Macros to tell you what operating system you are on
 * 
 * @link https://sourceforge.net/p/predef/wiki/OperatingSystems/
 */

/**
 * @brief Mac OS 9
 */
#if defined __APPLE__ || defined macintosh || defined Macintosh

#ifndef ON_MAC
#define ON_MAC
#endif

#ifdef MAC_VERSION
#define MAC_VERSION_BAD MAC_VERSION
#undef MAC_VERSION
#else
#define MAC_VERSION_BAD -1
#endif
#define MAC_VERSION 9

#endif

/**
 * @brief 
 */
#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "<project root directory undefined>"
#endif
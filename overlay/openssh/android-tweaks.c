#include "android-tweaks.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#ifdef WITH_OPENSSL
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#endif

//char *sshelper_user = NULL;

//static struct passwd android_pass = { "", "", 0, 0, "", "" };

/*
 * This is a stand-in for getpwuid() on an Android system
 * running without root authority, so the usual system call
 * cannot be made. All references to getpwuid() must be
 * redirected here.
 *
 */

static char datadir_root[] = "/data/ssh/root/";
static char datadir_shell[] = "/data/ssh/shell/";
static int android_uid_tweak_isLoaded = 0;
static uid_t android_uid_tweak_uidRoot=0;
static uid_t android_uid_tweak_uidShell=2000;

static void android_uid_tweak_load(void) {
	struct passwd *pw;
	if(android_uid_tweak_isLoaded)
		return;
	android_uid_tweak_isLoaded = 1;
	pw = getpwnam("root");
	if(pw != NULL)
		android_uid_tweak_uidRoot = pw->pw_uid;
	pw = getpwnam("shell");
	if(pw != NULL)
		android_uid_tweak_uidShell = pw->pw_uid;
}

static char* getUidDir(uid_t uid) {
	if(uid == android_uid_tweak_uidRoot)
		return datadir_root;
	else if(uid == android_uid_tweak_uidShell)
		return datadir_shell;
	return NULL;
}

struct passwd* getpwuida(uid_t uid) {
	char *new_dir;
	struct passwd *pw;
	android_uid_tweak_load();
	pw = getpwuid(uid);
	if(pw != NULL && (new_dir = getUidDir(uid)) != NULL) {
		pw->pw_dir = new_dir;
	}
	if(pw) {
		pw->pw_passwd="x";
#ifdef HAVE_STRUCT_PASSWD_PW_GECOS
		pw->pw_gecos="";
#endif
	}
	return pw;
}

struct passwd *getpwnama(const char *name) {
	char *new_dir;
	struct passwd *pw;
	android_uid_tweak_load();
	pw = getpwnam(name);
	if(pw != NULL && (new_dir = getUidDir(pw->pw_uid)) != NULL) {
		pw->pw_dir = new_dir;
	}
	if(pw) {
		pw->pw_passwd="x";
#ifdef HAVE_STRUCT_PASSWD_PW_GECOS
		pw->pw_gecos="";
#endif
	}
	return pw;
}

void endgrent(void) {
}

void endpwent(void) {
}

void setpwent(void) {
}

struct passwd *getpwent(void) {
	return NULL;
}

struct spwd *getspnam(const char *name) {
	static struct spwd sp;
	static char buf[1024];
	static char namebuf[64];
	static char passbuf[256];
	FILE *fp;
	char *saveptr, *token;

	fp = fopen("/data/ssh/etc/shadow", "r");
	if (fp == NULL)
		return NULL;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		buf[strcspn(buf, "\n")] = '\0';

		token = strtok_r(buf, ":", &saveptr);
		if (token == NULL || strcmp(token, name) != 0)
			continue;

		strncpy(namebuf, token, sizeof(namebuf) - 1);
		namebuf[sizeof(namebuf) - 1] = '\0';
		sp.sp_namp = namebuf;

		token = strtok_r(NULL, ":", &saveptr);
		if (token != NULL) {
			strncpy(passbuf, token, sizeof(passbuf) - 1);
			passbuf[sizeof(passbuf) - 1] = '\0';
		} else {
			passbuf[0] = '\0';
		}
		sp.sp_pwdp = passbuf;

		sp.sp_lstchg = -1;
		sp.sp_min = -1;
		sp.sp_max = -1;
		sp.sp_warn = -1;
		sp.sp_inact = -1;
		sp.sp_expire = -1;
		sp.sp_flag = -1;

		fclose(fp);
		return &sp;
	}

	fclose(fp);
	return NULL;
}

//
// int initgroups(const char *user, gid_t group) {
// 	return 0;
// }

//int getaddrinfo(const char *node, const char *service,
//               const struct addrinfo *hints,
//                struct addrinfo **res) {
//	return 0;
//}

// netdb:

//int getaddrinfo (const char *__restrict __name,
//			const char *__restrict __service,
//			const struct addrinfo *__restrict __req,
//			struct addrinfo **__restrict __pai) {
//	return 0;
//}

static char path_user[PATH_MAX] = ":";
static char path_suser[PATH_MAX] = ":";

const char* get_path_android(char root) {
	char *moddir;
	char *storage = root?path_suser:path_user;

	if(*storage == ':') {
		moddir = getenv("MODDIR");
		const char *formatter = root ?
		    "/system/bin:/system/xbin:/system/sbin:%s/usr/bin" :
		    "/system/bin:/system/xbin:%s/usr/bin";
		if(snprintf(storage, PATH_MAX, formatter, moddir) >= PATH_MAX) {
			storage[0] = '\0';
		}
	}
	return storage;
}


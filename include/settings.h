#ifndef SETTINGS_H
#define SETTINGS_H

#define CONFIG_FILE "settings.cfg"

#define SETTINGS_DEFAULT_GAME_PAIRS 16

GKeyFile *settings_init(void);
gboolean settings_deinit(GKeyFile *keyfile);

#endif

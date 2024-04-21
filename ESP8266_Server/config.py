import sys
import os
import json
from colorama import Fore

config_path = './config.json'

header_template = '''#ifndef {name}_H
#define {name}_H

{body}
#endif
'''

if not os.path.isfile(config_path):
    try:
        config = {
            'FORMAT_FS_ONSTARTUP' : False,
            'DISABLE_AUTHENTICATION' : False,
            'COOKIE_AUTHORIZATION_ONLY' : True,
            'SSID' : '',
            'PSWD' : '',
            'RESOURCE_PROVIDER' : '',
            'HTTP_PORT' : 80,
            'HTTPS_PORT' : 443,
            'DDNS_DOMAIN' : '',
            'DDNS_TOKEN' : '',
            'USER_NAME' : '',
            'PASSWORD' : '',
            'UP_PIN' : 0,
            'DOWN_PIN' : 2,
            'NR_OF_TIMINGS' : 6,
            'NR_OF_PROCESSES' : 6,
            'NR_OF_MESSAGES' : 10,
            'UP_TIME' : 27.0,
            'DOWN_TIME' : 26.0,
            'ACTIVATE_PIN' : 0,
            'DEACTIVATE_PIN' : 1
        }

        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
    except Exception as e:
        print(Fore.RED + 'Error creating config file: ' + str(e))
        sys.exit(1)

try:
    with open(config_path, 'r') as f:
        try:
            config = json.load(f)

            config_macros = ''

            for key, value in config.items():
                if type(value) == str or value is None:
                    config_macros += '#define {key} \"{value}\"\n'.format(key=key, value=value)
                else:
                    val = value

                    if type(value) == float:
                        val = '{value}f'.format(value=value)
                    elif type(value) == bool:
                        val = str(value).lower()

                    config_macros += '#define {key} {value}\n'.format(key=key, value=val)

            config_header = header_template.format(name='CONFIGURATION', body=config_macros)
            
            with open('./src/app/Configuration.h', 'w') as config_file:
                config_file.write(config_header)

        except json.JSONDecodeError as err:
            print(Fore.RED + 'Invalid configuration file.')
            sys.exit(1)
except Exception as e:
    print(Fore.RED + 'Error loading configuration file: ' + str(e))
    sys.exit(1)

body_template = '''#include <Arduino.h>

const char serverCert[] PROGMEM = R"EOF(
{cert})EOF";

const char serverKey[] PROGMEM = R"EOF(
{key})EOF";'''

try:
    with open('./key.pem') as key_file, open('./cert.pem') as cert_file, open('./src/app/Keys.h', 'w') as key_header_file:
        body = body_template.format(key=key_file.read(), cert=cert_file.read())
        config_header = header_template.format(name='KEYS', body=body)
        
        key_header_file.write(config_header)

except Exception as err:
    print(Fore.RED + 'Error loading key/cert file: ' + str(err))
    print(Fore.RED + 'Please place the correct key+cert files in the project root directory.')
    sys.exit(1)

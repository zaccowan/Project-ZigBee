#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>

#include <GS_SDHelper.h>

const char* WIFI_SSID = "cslab05WirelessTest";
const char* WIFI_PASSWORD = "password";

#define PROJECT_ID "project-zigbee"

#define CLIENT_EMAIL 	"esp32-logger@project-zigbee.iam.gserviceaccount.com"

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC1UvUuq/ttXGsi\nBmZC3Ac7EbcWrjB5OSCoVQ6Uoj7MoxUCtxXrR6jj8iBTbWDxqMCr6tuZffbqbyaf\n7J23DF/xh830plAMcQWr+wCr7nfcrqNLQtlw9DXQVFY4yGNbnnAqPI+7/I8FvrFV\nXmzgT3XwXRLLLBHiu+ha63H281BNeG8mYe4gPQf4MDzbGkpW1JR8MIxeAJhVC1UE\nHFOcG/8CUC5U+zCVG6jrcvO+jFJaF+mIHnqFDQ02y77bXCo2oLNLQdZKGXohslD7\n1XBUY13WVhvc36UweVfRbNSTTaySoAsLvhGMUccQPIdjCRZTAkM7WUQqDzeCWX8d\nApAFjd0DAgMBAAECggEAELpMgDfBhD9IauUoPfbKe/qQZ3k+Y5CPoLfoFnI5VOue\nnpeBYClGPXEy5j9q+CBXVgTeqXRjeRE0AQD1M9PibgfXSkClleA/Y7RORY5EZIl/\n6wuJv8/nvwZoxehGskcVle/sWq1cDG2hMnz5kmACQ2BywWeZZs9rCig6ZUvhFqRj\n1eLTnAVu/z9zUYF9Jc5Um3z9+GmHSwpBBqg78be4h1QmjVoNZZAfUHU6ziclMFCA\nVGjZ+1IL6h9Ensc49+mUMAXZ1zJ/aFOLAL9Nqd6vpEQceIVXU90tZd0H7HXtLK+5\nnaNhKycL/ILD+Aq41P1YGLmQ0IEDr6tKD3aJcAlwkQKBgQDmH+ob2VHAXS6kuBCh\nqYiyqmmpHnQlkgKB593oPQNgoeuy6w6hEHjBgKteJEK6+Ls+fRz3l7j2CseLt0+4\nnGGbNft7nZgsVNrtV3MYNQ8rok75ciVVOYk9BxPChpN4K77VeUSQd6tiDS6tde90\nK/czs7CtSufdEagVN4KqXYpuNwKBgQDJtlQw1VBDxA8eqM3tzxuCjrlcdNYNHetv\nvMBiT/Ovm47URLjfIy9xQhu09kGorcKZupc3Ya7nfNbycLHCAR1NBwvX4JJMv8KA\nKxqDxQp0+omvpKjftQhFV6riZOPMzqoRL1ojDw08zkOa+MHdGtdbfRZcA2ff35Z+\n3cSfWB6BlQKBgQDcYzwlp57vh04gWlczO9W6phHC8XNyd78HAjQ3MheF+AxusUa1\nYb0ENGHqKvftcECCu/d990Qf/nrUh55Fwx6HMs/Bt1ABRNLV8qY7TLVWqH7RzkuK\nryuL0rdN2Oxqbhq4/wZ3BOSRZuaJISVOMLzyOui9Tbt4sGw0xZDR7/ufSQKBgEqR\nF3Bx+tkCSmCYIIBP9qh94HIYUuTIcbd9vFTbn4cEZ3l66ii61hf1S07LVhxoF69C\nthHoD4NNuBwpotzIEq/d8ffc8dhjbyB4TrX1ilcSIwIGSdNhFeykauyERV357A6s\nRZL+RnN7AKzyVqBJAuG0TSa4r5aoDfdaPgx1w9k9AoGAHSCJLY5anmsNI8gLVVfU\nCyqMrA1PhrAIkQZbCsMZ0z+vX11hzWNzLnpYkqsftzu4J2m5u97cwuvNl/+GfhO7\n1Mz9G9pzRdl+df0Y6r/oYn8Dqh0mkMpRflj8k86LLZLDgmRN+KMIMQrhLH8Lk1vj\nzuu0Jc6zHlR3rQUBh8mYorc=\n-----END PRIVATE KEY-----\n";

const char spreadsheetId[] = "1C4SDqWgHrisyttjSN7e_ekchra03oGeIhan3pqTNbdo";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void tokenStatusCallback(TokenInfo info);

const char *ntpServer = "pool.ntp.org";

unsigned long epochTime;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  configTime(-18000, 3600, ntpServer);

  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION );

  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("connecting to wifi");
  while(WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  GSheet.setTokenCallback(tokenStatusCallback);
  GSheet.setPrerefreshSeconds(10 * 60);

  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

}

void loop() {
  bool ready = GSheet.ready();

  if( ready && millis() - lastTime > timerDelay ) {
    lastTime = millis();

    FirebaseJson response;

    Serial.println("\nAppend spreadsheet values...");
    Serial.println("-------------------------------");

    FirebaseJson valueRange;



    char timestr[20];
    time_t epochTime = time(NULL);
    strftime(timestr, sizeof(timestr), "%F %H:%M:%S", localtime(&epochTime));

    valueRange.add("majorDimension", "COLUMNS");
    valueRange.set("values/[0]/[0]", timestr);
    valueRange.set("values/[1]/[0]", 5678); //[col]/[row]

    bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!A1", &valueRange );
    if (success) {
      response.toString(Serial, true);
      valueRange.clear();
    }
    else {
      Serial.println(GSheet.errorReason());
    }

    Serial.println();
    Serial.println(ESP.getFreeHeap());
  }

}

void tokenStatusCallback(TokenInfo info) {
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
  }


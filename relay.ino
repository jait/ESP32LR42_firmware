

#define RELAY_SHUTOFF_TIME 15000 // milliseconds
int RelayOffTimer = 0;

const int NUM_RELAYS = 4;
const int _relays[NUM_RELAYS] = {Rly1, Rly2, Rly3, Rly4};

bool setRelay(char rly, char state)
{
  int i;
  if (state == '0' || state == 0)
    state = LOW;
  else if (state == '1' || state == 1)
    state = HIGH;
  else
    return false;

  // ascii to index
  if (rly >= '1')
    rly -= '1';

  if (rly < 0 || rly >= NUM_RELAYS)
    return false;

  digitalWrite(_relays[rly], state);
  // start timer if some relay was turned on
  if (state == HIGH && RelayOffTimer == 0) {
    RelayOffTimer = millis() + RELAY_SHUTOFF_TIME;
  } else if (state == LOW && RelayOffTimer != 0) {
      // remove timer if all relays are off
      bool allOff = true;
      for (i = 0; i < NUM_RELAYS; i++) {
        if (getRelay(i) > 0) {
          allOff = false;
          break;
        }
      }
      if (allOff) {
        RelayOffTimer = 0;
      }
  }

  return true;
}

char getRelay(char rly)
{
  // ascii to index
  if (rly >= '1')
    rly -= '1';

  if (rly < 0 || rly >= NUM_RELAYS)
    return -1;

  return digitalRead(_relays[rly]);
}

void modeRelay()
{
  int i;
  if (RelayOffTimer == 0)
    return;

  // check if we have to shut off relays
  if (millis() > RelayOffTimer) {
    for (i = 0; i < NUM_RELAYS; i++) {
      setRelay(i, 0);
    }
    RelayOffTimer = 0;
  }
}
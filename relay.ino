

const int NUM_RELAYS = 4;
const int _relays[NUM_RELAYS] = {Rly1, Rly2, Rly3, Rly4};

bool setRelay(char rly, char state)
{
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
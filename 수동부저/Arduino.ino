int buzzerPin = 9;
int songLength = 24;

char notes[] = "ffgga gffeedd efgfe d";                                                 // Notes
int beats[] = {4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 4}; // Corresponding beats
int tempo = 80;                                                                         // Tempo

void setup()
{
  pinMode(buzzerPin, OUTPUT);
}

void loop()
{
  int duration;
  for (int i = 0; i < songLength; i++)
  {
    duration = beats[i] * tempo;
    if (notes[i] == ' ')
    {
      noTone(buzzerPin);
      delay(duration);
    }
    else
    {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);
    }
    delay(tempo / 10);
  }
  delay(5000); // Delay between loops
}

int frequency(char note)
{
  int i;
  const int numNotes = 7;
  char names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b'};
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494};
  for (i = 0; i < numNotes; i++)
  {
    if (names[i] == note)
    {
      return frequencies[i];
    }
  }
  return 0;
}

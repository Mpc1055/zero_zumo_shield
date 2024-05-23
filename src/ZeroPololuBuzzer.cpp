#include "ZeroPololuBuzzer.h"
#include "Tone.h"
#include "sam.h"
#include <cctype> // Include this header for isdigit

#define BUZZER_PIN A0 // Define the pin number for PA02

unsigned char buzzerInitialized = 0;
volatile unsigned char buzzerFinished = 1;
const char* volatile buzzerSequence = 0;

static volatile unsigned int buzzerTimeout = 0;
static volatile char play_mode_setting = PLAY_AUTOMATIC;

static volatile unsigned char use_program_space;
static volatile unsigned char octave = 4;
static volatile unsigned int whole_note_duration = 2000;
static volatile unsigned int note_type = 4;
static volatile unsigned int duration = 500;
static volatile unsigned int volume = 15;
static volatile unsigned char staccato = 0;
static volatile unsigned char staccato_rest_duration;

static void nextNote();



void PololuBuzzer::playFrequency(unsigned int freq, unsigned int dur, unsigned char volume) {
    tone(BUZZER_PIN, freq, dur);
    delay(dur);
//    delayMicroseconds(duration[currentIdx]);

}

void PololuBuzzer::play(const char *notes) {
    buzzerSequence = notes;
    use_program_space = 0;
    staccato_rest_duration = 0;
    nextNote();
}

void PololuBuzzer::playFromProgramSpace(const char *notes_p) {
    buzzerSequence = notes_p;
    use_program_space = 1;
    staccato_rest_duration = 0;
    nextNote();
}

void PololuBuzzer::stopPlaying() {
    noTone(BUZZER_PIN);
    buzzerFinished = 1;
    buzzerSequence = 0;
}

unsigned char PololuBuzzer::isPlaying() {
    return !buzzerFinished || buzzerSequence != 0;
}

void PololuBuzzer::playMode(unsigned char mode) {
    play_mode_setting = mode;
}

static char currentCharacter() {
    char c = 0;
    do {
        if (use_program_space) {
            c = pgm_read_byte(buzzerSequence);
        } else {
            c = *buzzerSequence;
        }

        if (c >= 'A' && c <= 'Z') {
            c += 'a' - 'A';
        }
    } while (c == ' ' && (buzzerSequence++));

    return c;
}

static unsigned int getNumber() {
    unsigned int arg = 0;
    char c = currentCharacter();
    while (isdigit(c)) {  // Use isdigit to check if the character is a digit
        arg *= 10;
        arg += c - '0';
        buzzerSequence++;
        c = currentCharacter();
    }
    return arg;
}

static void nextNote() {
    unsigned char note = 0;
    unsigned char rest = 0;
    unsigned char tmp_octave = octave;
    unsigned int tmp_duration;
    unsigned int dot_add;

    char c;

    if (staccato && staccato_rest_duration) {
        PololuBuzzer::playNote(SILENT_NOTE, staccato_rest_duration, 0);
        staccato_rest_duration = 0;
        return;
    }

parse_character:
    c = currentCharacter();
    buzzerSequence++;

    switch (c) {
        case '>':
            tmp_octave++;
            goto parse_character;
        case '<':
            tmp_octave--;
            goto parse_character;
        case 'a':
            note = NOTE_A(0);
            break;
        case 'b':
            note = NOTE_B(0);
            break;
        case 'c':
            note = NOTE_C(0);
            break;
        case 'd':
            note = NOTE_D(0);
            break;
        case 'e':
            note = NOTE_E(0);
            break;
        case 'f':
            note = NOTE_F(0);
            break;
        case 'g':
            note = NOTE_G(0);
            break;
        case 'l':
            note_type = getNumber();
            duration = whole_note_duration / note_type;
            goto parse_character;
        case 'm':
            if (currentCharacter() == 'l') {
                staccato = false;
            } else {
                staccato = true;
                staccato_rest_duration = 0;
            }
            buzzerSequence++;
            goto parse_character;
        case 'o':
            octave = tmp_octave = getNumber();
            goto parse_character;
        case 'r':
            rest = 1;
            break;
        case 't':
            whole_note_duration = 60 * 400 / getNumber() * 10;
            duration = whole_note_duration / note_type;
            goto parse_character;
        case 'v':
            volume = getNumber();
            goto parse_character;
        case '!':
            octave = 4;
            whole_note_duration = 2000;
            note_type = 4;
            duration = 500;
            volume = 15;
            staccato = 0;
            staccato_rest_duration = 0;
            goto parse_character;
        default:
            buzzerSequence--;
            return;
    }

    if (!rest) {
        switch (currentCharacter()) {
            case '#':
            case '+':
                buzzerSequence++;
                note++;
                break;
            case '-':
                buzzerSequence++;
                note--;
                break;
        }

        note += tmp_octave * 12;
    }

    tmp_duration = duration;
    if (isdigit(currentCharacter())) {
        tmp_duration = whole_note_duration / getNumber();
    }

    dot_add = tmp_duration / 2;
    while (currentCharacter() == '.') {
        buzzerSequence++;
        tmp_duration += dot_add;
        dot_add /= 2;
    }

    if (staccato) {
        staccato_rest_duration = tmp_duration / 2;
        tmp_duration -= staccato_rest_duration;
    }

    if (rest) {
        PololuBuzzer::playNote(SILENT_NOTE, tmp_duration, 0);
    } else {
        PololuBuzzer::playNote(note, tmp_duration, volume);
    }
}

void PololuBuzzer::playNote(unsigned char note, unsigned int dur,
                 unsigned char volume) {
  unsigned int freq = 0;
  unsigned char offset_note = note - 16;

  if (note == SILENT_NOTE || volume == 0)
  {
    freq = 1000;  // silent notes => use 1kHz freq (for cycle counter)
    playFrequency(freq, dur, 0);
    return;
  }

  if (note <= 16)
    offset_note = 0;
  else if (offset_note > 95)
    offset_note = 95;

  unsigned char exponent = offset_note / 12;

  // frequency table for the lowest 12 allowed notes
  //   frequencies are specified in tenths of a Hertz for added resolution
  switch (offset_note - exponent * 12)  // equivalent to (offset_note % 12)
  {
    case 0:        // note E1 = 41.2 Hz
      freq = 412;
      break;
    case 1:        // note F1 = 43.7 Hz
      freq = 437;
      break;
    case 2:        // note F#1 = 46.3 Hz
      freq = 463;
      break;
    case 3:        // note G1 = 49.0 Hz
      freq = 490;
      break;
    case 4:        // note G#1 = 51.9 Hz
      freq = 519;
      break;
    case 5:        // note A1 = 55.0 Hz
      freq = 550;
      break;
    case 6:        // note A#1 = 58.3 Hz
      freq = 583;
      break;
    case 7:        // note B1 = 61.7 Hz
      freq = 617;
      break;
    case 8:        // note C2 = 65.4 Hz
      freq = 654;
      break;
    case 9:        // note C#2 = 69.3 Hz
      freq = 693;
      break;
    case 10:      // note D2 = 73.4 Hz
      freq = 734;
      break;
    case 11:      // note D#2 = 77.8 Hz
      freq = 778;
      break;
  }

  if (exponent < 7)
  {
    freq = freq << exponent;  // frequency *= 2 ^ exponent
    if (exponent > 1)      // if the frequency is greater than 160 Hz
      freq = (freq + 5) / 10;  //   we don't need the extra resolution
    else
      freq += DIV_BY_10;    // else keep the added digit of resolution
  }
  else
    freq = (freq * 64 + 2) / 5;  // == freq * 2^7 / 10 without int overflow

  if (volume > 15)
    volume = 15;
  playFrequency(freq, dur, volume);  // set buzzer this freq/duration

}

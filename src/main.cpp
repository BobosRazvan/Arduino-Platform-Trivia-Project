#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

#include "HD44780.hpp"
#include "libADC.hpp"
#include "uart_buffer.hpp"

int main(void)
{

  LCD_Initalize();
  uart_init(9600, 0);
  ADC_Init();
  sei();

  uint16_t user_input, old_user_input = 1023;
  char buf[20];
  char state;
  uint8_t count = 0;

  const char *questions[] = {
      "How Much Percentage Of Earth’s Surface Is Covered By Water?",
      "What was Martin Luther King Jr famous speech commonly referred to as?",
      "Who directed the film 'Schindler’s List'?",
      "In the film 'The Shawshank Redemption,' the name of the main character played by Tim Robbins is?",
      "Who won the Academy Award for Best Actress for her role in the film 'La La Land'?",
      "Which is the highest-grossing animated film of all time?",
      "In which year was the first Academy Awards ceremony held?",
      "Who played the character of Jack Dawson in the film 'Titanic'?",
      "What is the capital of France?",
      "Who wrote 'Romeo and Juliet'?"};

  const char *possibleAnswers[][4] = {
      {"51%", "61%", "91%", "71%"},
      {"I have a dream", "Freedom Fighters", "Dead and Evil", "Rise Up"},
      {"Steven Spielberg", "Martin Scorsese", "Michael Jackson", "Christopher Nolan"},
      {"Andy Dufresne", "Red", "Ellis Boyd 'Red' Redding", "Warden Norton"},
      {"Emma Stone", "Meryl Streep", "Jennifer Lawrence", "Natalie Portman"},
      {"Toy Story 4", "Avatar", "Frozen 2", "The Lion King"},
      {"1927", "1932", "1929", "1951"},
      {"Leonardo DiCaprio", "Tom Hanks", "Brad Pitt", "Matt Damon"},
      {"Berlin", "Madrid", "Paris", "Rome"},
      {"Charles Dickens", "William Shakespeare", "Jane Austen", "Mark Twain"}};

  const char correctAnswers[] = {'d', 'a', 'a', 'a', 'a', 'd', 'b', 'a', 'c', 'b'};
  int index = 0;
  int game_started = 0;
  int question_sent = 0;

  while (1)
  {
    if (game_started == 0)
    {
      LCD_WriteCommand(HD44780_CLEAR);
      _delay_ms(10);
      LCD_GoTo(0, 0);
      sprintf(buf, "Trivia Game");
      LCD_WriteText(buf);
      LCD_GoTo(1, 1);
      sprintf(buf, "Use L, U, D, R");
      LCD_WriteText(buf);
      _delay_ms(2000);

      LCD_WriteCommand(HD44780_CLEAR);
      _delay_ms(10);
      LCD_GoTo(0, 0);
      sprintf(buf, "L=a, U=b,");
      LCD_WriteText(buf);
      LCD_GoTo(1, 1);
      sprintf(buf, "D=c, R=d;");
      LCD_WriteText(buf);
      _delay_ms(2000);

      LCD_WriteCommand(HD44780_CLEAR);
      _delay_ms(10);
      LCD_GoTo(0, 0);
      sprintf(buf, "Please answer");
      LCD_WriteText(buf);
      LCD_GoTo(1, 1);
      sprintf(buf, "first question.");
      LCD_WriteText(buf);
      _delay_ms(2000);

      game_started = 1;
      question_sent = 0;
    }
    else
    {
      if (index < 10)
      {

        if (question_sent == 0)
        {
          char buf[200];
          sprintf(buf, "Question no %d:\n", index);

          uart_send_string((uint8_t *)buf);
          uart_send_string((uint8_t *)questions[index]);
          uart_send_string((uint8_t *)"\n");

          sprintf(buf, "a) %s\nb) %s\nc) %s\nd) %s\n",
                  possibleAnswers[index][0],
                  possibleAnswers[index][1],
                  possibleAnswers[index][2],
                  possibleAnswers[index][3]);
          uart_send_string((uint8_t *)buf);

          question_sent = 1;
        }
        user_input = ADC_conversion();
        if (user_input > 900)
        {
          old_user_input = user_input;
        }
        else if (abs(user_input - old_user_input) > 50)
        {

          if (user_input < 100)
          {
            state = 'd';
          }
          else if (user_input < 250)
          {
            state = 'b';
          }
          else if (user_input < 350)
          {
            state = 'c';
          }
          else if (user_input < 500)
          {
            state = 'a';
          }

          if (state == correctAnswers[index])
          {
            count++;
            LCD_WriteCommand(HD44780_CLEAR);
            _delay_ms(10);
            LCD_GoTo(0, 0);
            sprintf(buf, "%c correct!", state);
            LCD_WriteText(buf);
          }
          else
          {
            LCD_WriteCommand(HD44780_CLEAR);
            _delay_ms(10);
            LCD_GoTo(0, 0);
            sprintf(buf, "%c incorrect(%c)", state, correctAnswers[index]);
            LCD_WriteText(buf);
          }

          LCD_GoTo(1, 1);
          sprintf(buf, "score: %d/%d", count, index + 1);
          LCD_WriteText(buf);
          _delay_ms(1000);

          index++;
          old_user_input = 1023;
          question_sent = 0;
        }
      }
      else
      {
        LCD_WriteCommand(HD44780_CLEAR);
        _delay_ms(10);
        LCD_GoTo(0, 0);
        sprintf(buf, "END OF THE GAME!");
        LCD_WriteText(buf);
        LCD_GoTo(1, 1);
        sprintf(buf, "Final sc.: %d/%d", count, index);
        LCD_WriteText(buf);
        _delay_ms(4000);

        LCD_WriteCommand(HD44780_CLEAR);
        _delay_ms(10);
        LCD_GoTo(0, 0);
        sprintf(buf, "Press SELECT");
        LCD_WriteText(buf);
        LCD_GoTo(1, 1);
        sprintf(buf, "to play again.");
        LCD_WriteText(buf);
        _delay_ms(2000);

        do
        {
          user_input = ADC_conversion();
          _delay_ms(200);
        } while (user_input > 750);
        index = 0;
        count = 0;
        old_user_input = 1023;
        game_started = 0;
      }
    }
  }
}

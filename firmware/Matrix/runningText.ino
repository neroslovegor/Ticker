// работа с бегущим текстом


#define TEXT_DIRECTION 1  // 1 - по горизонтали, 0 - по вертикали
#define MIRR_V 0          // отразить текст по вертикали (0 / 1)
#define MIRR_H 1          // отразить текст по горизонтали (0 / 1)

#define TEXT_HEIGHT 0     // высота, на которой бежит текст (от низа матрицы)
#define LET_WIDTH 5       // ширина буквы шрифта
#define LET_HEIGHT 8      // высота буквы шрифта
#define SPACE 1           // пробел



int offset = WIDTH;

void fillString(String text, byte color) {
  if (loadingFlag) {
    offset = WIDTH;   // перемотка в правый край
    loadingFlag = false;
    fullTextFlag = false;
  }


  if (scrollTimer.isReady()) {
    if (showText) {
      matrix.fillScreen(LOW);
      byte i = 0, j = 0;
      while (text[i] != '\0') {
        if ((byte)text[i] > 191) {    // работаем с русскими буквами!
          i++;
        } else {
          drawLetter(j, text[i], offset + j * (LET_WIDTH + SPACE), color);
          i++;
          j++;
        }
      }

      offset--;
      if (offset < -j * (LET_WIDTH + SPACE)) {    // строка убежала
        offset = WIDTH + 3;
        if (!autoFlag) showText = false;
      }
      matrix.write();
    }
  }
}

void drawLetter(uint8_t index, uint8_t letter, int16_t offset, byte color) {
  int8_t start_pos = 0, finish_pos = LET_WIDTH;
  
  if (offset < -LET_WIDTH || offset > WIDTH) return;
  if (offset < 0) start_pos = -offset;
  if (offset > (WIDTH - LET_WIDTH)) finish_pos = WIDTH - offset;

  for (byte i = start_pos; i < finish_pos; i++) {
    int thisByte;
    if (MIRR_V) thisByte = getFont((byte)letter, LET_WIDTH - 1 - i);
    else thisByte = getFont((byte)letter, i);

    for (byte j = 0; j < LET_HEIGHT; j++) {
      boolean thisBit;

      if (MIRR_H) thisBit = thisByte & (1 << j);
      else thisBit = thisByte & (1 << (LET_HEIGHT - 1 - j));

      // рисуем столбец (i - горизонтальная позиция, j - вертикальная)
      if (TEXT_DIRECTION) {
        if (thisBit) matrix.drawPixel(offset + i, TEXT_HEIGHT + j, 1);
        else matrix.drawPixel(offset + i, TEXT_HEIGHT + j, 0);
      } else {
        if (thisBit) matrix.drawPixel(i, offset + TEXT_HEIGHT + j, 1);
        else matrix.drawPixel(i, offset + TEXT_HEIGHT + j, 0);
      }
    }
  }
}



// интерпретатор кода символа в массиве fontHEX 
uint8_t getFont(uint8_t font, uint8_t row) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива
  if (font <= 90) return pgm_read_byte(&(fontHEX[font][row]));     // для английских букв и символов
  else if (font >= 112 && font <= 159) {    // и пизд*ц ждя русских
    return pgm_read_byte(&(fontHEX[font - 17][row]));
  } else if (font >= 96 && font <= 111) {
    return pgm_read_byte(&(fontHEX[font + 47][row]));
  }
}

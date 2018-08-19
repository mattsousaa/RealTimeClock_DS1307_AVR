# Criação de um relógio de tempo real usando display LCD e o dispositivo DS1307

O código acima está modulado para melhor entendimento. Primeiramente é feito uma lógica por máquina de estados para o funcionamento
do display LCD. Para economia de GPIO's no microcontrolador é usado o LCD no modo de envio de dados a cada Nibble. Em seguida é 
implementado as funções em I2C para recebimento e envio de endereços para a memória do DS1307. O relógio mostra o dia da semana, 
hora, mês e ano. O usuário tem a opção de modificar a data e a hora utilizando a UART, digitando a data e hora da seguinte forma:

**hour/min/sec/week_day/month_day/month/year**

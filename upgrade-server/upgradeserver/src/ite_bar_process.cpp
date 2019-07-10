#include "ite_bar_process.h"
#include "string.h"
#include "stdio.h"
#include <assert.h>
#include  <unistd.h>
#include <stdlib.h>

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_Bar_Process
 *          C���Խ�����
 * @param[in]  bar_name  ��ʾ����������(����ΪNULL)
 * @param[in]  number    ����ռ��%   [0--100]
 */
/* --------------------------------------------------------------------------*/
void ITE_Bar_Process(char *bar_name, unsigned char number)
{
    char buf[103];
    if (number > 100)
    {
        return;
    }
    memset(buf, ' ', sizeof(buf));
    buf[0] = '[';
    buf[101] = ']';
    buf[102] = '\0';
    int i = 0;
    char index[6] = "-\\|/\0";
    while (i < number)
    {
        i++;
        buf[i] = '=';
    }
    if (bar_name)
    {
        printf("%s \t\t %s [%d%%][%c]\r", bar_name, buf, i, index[i % 4]);
    }
    else
    {
        printf("\t\t %s [%d%%][%c]\r", buf, i, index[i % 4]);
    }
    fflush(stdout);//ˢ�»�����
    printf("\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define D_CRC_TABLE_SIZE 256
/*******************************************************************
 * Locall Function Declaration
 *******************************************************************/
static uint32_t crc_bitrev(uint32_t poly, uint32_t bw);
static void crc_create_table(uint32_t poly, uint32_t bw, uint32_t *table);
static uint32_t crc_calc( void* table, uint32_t crc, void* input, uint32_t len, uint32_t bw, bool input_rev, bool output_rev );
/*******************************************************************
 *  Global variable
 *******************************************************************/
static uint32_t g_crc32_table[D_CRC_TABLE_SIZE] = {0};
/*****************************************************************************
* Function  : crc_bitrev
* Purpose   : Bit reversal
* Relation  :
*
* Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*      poly             uint32_t                  In        Polynomial
*       bw              uint32_t                  In        Bit width(8,16,24,32 ...)
*
* Return:
* Note: 
*******************************************************************************/
static uint32_t crc_bitrev(uint32_t poly, uint32_t bw)
{
    uint32_t var = 0;

    for (uint32_t i = 0; i < bw; i++)
    {
        if (poly & 0x01)
        {
            var |= 1 << (bw - 1 - i);
        }
        poly >>= 1;
    }
    return var;
}
/*****************************************************************************
* Function  : crc_create_table
* Purpose   : create crc table
* Relation  :
*
* Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       poly            uint32_t                  In        Polynomial
*       bw              uint32_t                  In        Bit width(8,16,24,32 ...)
*      table            uint32_t*                 In        crc table
*
* Return:
* Note: 
*******************************************************************************/
static void crc_create_table(uint32_t poly, uint32_t bw, uint32_t *table)
{
    if (table)
    {
        uint32_t c;
        poly = crc_bitrev(poly, bw);
        for (uint32_t i = 0; i < D_CRC_TABLE_SIZE; i++)
        {
            c = i;
            for (uint32_t j = 0; j < 8; j++)
            {
                if (c & 1)
                {
                    c = poly ^ (c >> 1);
                }
                else
                {
                    c = c >> 1;
                }
            }
            table[i] = c;
        }
    }
}
/*****************************************************************************
* Function  : crc_calc
* Purpose   : crc calculate
* Relation  :
*
* Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*      table               void*                 In           crc table
*      crc                uint32_t               In           crc init value
*      input               void*                 In           source data
*      len               uint32_t                In           source data len
*      bw                uint32_t                In           Bit width(8,16,24,32 ...)
*      input_rev           bool                  In           ref in
*      output_rev          bool                  In           ref out
*
* Return:
* Note: 
*******************************************************************************/
static uint32_t crc_calc( void* table, uint32_t crc, void* input, uint32_t len, uint32_t bw, bool input_rev, bool output_rev )
{
    uint8_t index = 0;
    uint8_t* pch = (uint8_t*)input;
    uint32_t* crc_table_ptr = (uint32_t*)table;

    for (uint32_t i = 0; i < len; i++)
    {
        if (!input_rev)
        {
            *pch = crc_bitrev(*pch, 8);
        }
        index = (uint8_t)(crc^*pch);
        crc = (crc >> 8) ^ crc_table_ptr[index];
        pch++;
    }
    if (!output_rev)
    {
        crc = crc_bitrev(crc, bw);
    }
    return crc;
}
/*****************************************************************************
* Function  : main
* Purpose   : 
* Relation  :
*
* Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                N/A                   N/A        N/A
*
* Return:
* Note: 
*******************************************************************************/
int main()
{
    crc_create_table(0xF4ACFB13,32, g_crc32_table);
    uint8_t data[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    uint32_t crc_init = 0xFFFFFFFF;
    uint32_t crc32 = crc_calc(g_crc32_table, crc_init, data, sizeof(data), 32, true, true);
    crc32 = crc32 ^ 0xFFFFFFFF;
    printf("crc:0x%08X\n",crc32);
}
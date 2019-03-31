#include "general_sync.h"
float RM_Objects[108];
float getValue(float previousValue, float nextValue, float currentRow, int previousRow, int nextRow, int interpol) {
	float t = (currentRow - previousRow) / (nextRow - previousRow);
    if(interpol == 0) {
        t = 0;
    }
	return previousValue * (1.0f-t) + nextValue * t;
}
void SyndDataHandle(float row, float array[][3], int array_length, int index) {
    for (int i = 0; i < array_length - 1; i++) {
        if (array[i][0] > row && array[i+1][0] < row) {
            float interpolation = array[i][2];
            float value = getValue(array[i][2], array[i+1][2], row, array[i][0], array[i+1][0], array[i][1]);
            RM_Objects[index] = value;
        }
    }
}
void AllSyncDataHandle(float row) {
   SyndDataHandle(row, rm0_position_x, 13, 0); 
   SyndDataHandle(row, rm0_position_y, 10, 1); 
   SyndDataHandle(row, rm0_position_z, 13, 2); 
   SyndDataHandle(row, rm0_scale_x, 3, 3); 
   SyndDataHandle(row, rm0_scale_y, 3, 4); 
   SyndDataHandle(row, rm0_scale_z, 3, 5); 
   SyndDataHandle(row, rm0_rotation_x, 12, 6); 
   SyndDataHandle(row, rm0_rotation_y, 9, 7); 
   SyndDataHandle(row, rm0_rotation_z, 11, 8); 
   SyndDataHandle(row, rm1_position_x, 11, 9); 
   SyndDataHandle(row, rm1_position_y, 10, 10); 
   SyndDataHandle(row, rm1_position_z, 13, 11); 
   SyndDataHandle(row, rm1_scale_x, 3, 12); 
   SyndDataHandle(row, rm1_scale_y, 3, 13); 
   SyndDataHandle(row, rm1_scale_z, 3, 14); 
   SyndDataHandle(row, rm1_rotation_x, 13, 15); 
   SyndDataHandle(row, rm1_rotation_y, 11, 16); 
   SyndDataHandle(row, rm1_rotation_z, 12, 17); 
   SyndDataHandle(row, rm2_position_x, 7, 18); 
   SyndDataHandle(row, rm2_position_y, 5, 19); 
   SyndDataHandle(row, rm2_position_z, 7, 20); 
   SyndDataHandle(row, rm2_scale_x, 2, 21); 
   SyndDataHandle(row, rm2_scale_y, 2, 22); 
   SyndDataHandle(row, rm2_scale_z, 2, 23); 
   SyndDataHandle(row, rm2_rotation_x, 6, 24); 
   SyndDataHandle(row, rm2_rotation_y, 7, 25); 
   SyndDataHandle(row, rm2_rotation_z, 7, 26); 
   SyndDataHandle(row, rm3_position_x, 6, 27); 
   SyndDataHandle(row, rm3_position_y, 5, 28); 
   SyndDataHandle(row, rm3_position_z, 7, 29); 
   SyndDataHandle(row, rm3_scale_x, 2, 30); 
   SyndDataHandle(row, rm3_scale_y, 2, 31); 
   SyndDataHandle(row, rm3_scale_z, 2, 32); 
   SyndDataHandle(row, rm3_rotation_x, 6, 33); 
   SyndDataHandle(row, rm3_rotation_y, 6, 34); 
   SyndDataHandle(row, rm3_rotation_z, 6, 35); 
   SyndDataHandle(row, rm4_position_x, 5, 36); 
   SyndDataHandle(row, rm4_position_y, 5, 37); 
   SyndDataHandle(row, rm4_position_z, 6, 38); 
   SyndDataHandle(row, rm4_scale_x, 1, 39); 
   SyndDataHandle(row, rm4_scale_y, 1, 40); 
   SyndDataHandle(row, rm4_scale_z, 1, 41); 
   SyndDataHandle(row, rm4_rotation_x, 6, 42); 
   SyndDataHandle(row, rm4_rotation_y, 4, 43); 
   SyndDataHandle(row, rm4_rotation_z, 5, 44); 
   SyndDataHandle(row, rm5_position_x, 6, 45); 
   SyndDataHandle(row, rm5_position_y, 5, 46); 
   SyndDataHandle(row, rm5_position_z, 6, 47); 
   SyndDataHandle(row, rm5_scale_x, 1, 48); 
   SyndDataHandle(row, rm5_scale_y, 1, 49); 
   SyndDataHandle(row, rm5_scale_z, 1, 50); 
   SyndDataHandle(row, rm5_rotation_x, 6, 51); 
   SyndDataHandle(row, rm5_rotation_y, 6, 52); 
   SyndDataHandle(row, rm5_rotation_z, 5, 53); 
   SyndDataHandle(row, rm6_position_x, 6, 54); 
   SyndDataHandle(row, rm6_position_y, 4, 55); 
   SyndDataHandle(row, rm6_position_z, 5, 56); 
   SyndDataHandle(row, rm6_scale_x, 1, 57); 
   SyndDataHandle(row, rm6_scale_y, 1, 58); 
   SyndDataHandle(row, rm6_scale_z, 1, 59); 
   SyndDataHandle(row, rm6_rotation_x, 4, 60); 
   SyndDataHandle(row, rm6_rotation_y, 5, 61); 
   SyndDataHandle(row, rm6_rotation_z, 4, 62); 
   SyndDataHandle(row, rm7_position_x, 5, 63); 
   SyndDataHandle(row, rm7_position_y, 5, 64); 
   SyndDataHandle(row, rm7_position_z, 6, 65); 
   SyndDataHandle(row, rm7_scale_x, 1, 66); 
   SyndDataHandle(row, rm7_scale_y, 1, 67); 
   SyndDataHandle(row, rm7_scale_z, 1, 68); 
   SyndDataHandle(row, rm7_rotation_x, 6, 69); 
   SyndDataHandle(row, rm7_rotation_y, 6, 70); 
   SyndDataHandle(row, rm8_position_x, 5, 71); 
   SyndDataHandle(row, rm8_position_y, 5, 72); 
   SyndDataHandle(row, rm8_position_z, 6, 73); 
   SyndDataHandle(row, rm8_scale_x, 1, 74); 
   SyndDataHandle(row, rm8_scale_y, 1, 75); 
   SyndDataHandle(row, rm8_scale_z, 1, 76); 
   SyndDataHandle(row, rm8_rotation_x, 6, 77); 
   SyndDataHandle(row, rm8_rotation_y, 5, 78); 
   SyndDataHandle(row, rm8_rotation_z, 6, 79); 
   SyndDataHandle(row, rm9_position_x, 6, 80); 
   SyndDataHandle(row, rm9_position_y, 5, 81); 
   SyndDataHandle(row, rm9_position_z, 6, 82); 
   SyndDataHandle(row, rm9_scale_x, 1, 83); 
   SyndDataHandle(row, rm9_scale_y, 1, 84); 
   SyndDataHandle(row, rm9_scale_z, 1, 85); 
   SyndDataHandle(row, rm9_rotation_x, 4, 86); 
   SyndDataHandle(row, rm9_rotation_y, 6, 87); 
   SyndDataHandle(row, rm9_rotation_z, 5, 88); 
   SyndDataHandle(row, rm10_position_x, 5, 89); 
   SyndDataHandle(row, rm10_position_y, 6, 90); 
   SyndDataHandle(row, rm10_position_z, 6, 91); 
   SyndDataHandle(row, rm10_scale_x, 1, 92); 
   SyndDataHandle(row, rm10_scale_y, 1, 93); 
   SyndDataHandle(row, rm10_scale_z, 1, 94); 
   SyndDataHandle(row, rm10_rotation_x, 5, 95); 
   SyndDataHandle(row, rm10_rotation_y, 6, 96); 
   SyndDataHandle(row, rm10_rotation_z, 6, 97); 
   SyndDataHandle(row, rm11_position_x, 6, 98); 
   SyndDataHandle(row, rm11_position_y, 6, 99); 
   SyndDataHandle(row, rm11_position_z, 6, 100); 
   SyndDataHandle(row, rm11_scale_x, 1, 101); 
   SyndDataHandle(row, rm11_scale_y, 1, 102); 
   SyndDataHandle(row, rm11_scale_z, 1, 103); 
   SyndDataHandle(row, rm11_rotation_x, 6, 104); 
   SyndDataHandle(row, rm11_rotation_y, 5, 105); 
   SyndDataHandle(row, rm11_rotation_z, 6, 106); 
   SyndDataHandle(row, rm7_rotation_z, 3, 107); 
} 


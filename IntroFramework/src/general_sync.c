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
        if (array[i][0] >= row && array[i+1][0] < row) {
            float interpolation = array[i][2];
            float value = getValue(array[i][2], array[i+1][2], row, array[i][0], array[i+1][0], array[i][1]);
            RM_Objects[index] = value;
        }
    }
}
void AllSyncDataHandle(float row) {
   SyndDataHandle(row, rm0_position_x, 102, 0); 
   SyndDataHandle(row, rm0_position_y, 85, 1); 
   SyndDataHandle(row, rm0_position_z, 95, 2); 
   SyndDataHandle(row, rm0_scale_x, 3, 3); 
   SyndDataHandle(row, rm0_scale_y, 3, 4); 
   SyndDataHandle(row, rm0_scale_z, 3, 5); 
   SyndDataHandle(row, rm0_rotation_x, 91, 6); 
   SyndDataHandle(row, rm0_rotation_y, 92, 7); 
   SyndDataHandle(row, rm0_rotation_z, 90, 8); 
   SyndDataHandle(row, rm1_position_x, 100, 9); 
   SyndDataHandle(row, rm1_position_y, 85, 10); 
   SyndDataHandle(row, rm1_position_z, 97, 11); 
   SyndDataHandle(row, rm1_scale_x, 3, 12); 
   SyndDataHandle(row, rm1_scale_y, 3, 13); 
   SyndDataHandle(row, rm1_scale_z, 3, 14); 
   SyndDataHandle(row, rm1_rotation_x, 93, 15); 
   SyndDataHandle(row, rm1_rotation_y, 96, 16); 
   SyndDataHandle(row, rm1_rotation_z, 96, 17); 
   SyndDataHandle(row, rm2_position_x, 51, 18); 
   SyndDataHandle(row, rm2_position_y, 42, 19); 
   SyndDataHandle(row, rm2_position_z, 50, 20); 
   SyndDataHandle(row, rm2_scale_x, 2, 21); 
   SyndDataHandle(row, rm2_scale_y, 2, 22); 
   SyndDataHandle(row, rm2_scale_z, 2, 23); 
   SyndDataHandle(row, rm2_rotation_x, 48, 24); 
   SyndDataHandle(row, rm2_rotation_y, 48, 25); 
   SyndDataHandle(row, rm2_rotation_z, 49, 26); 
   SyndDataHandle(row, rm3_position_x, 50, 27); 
   SyndDataHandle(row, rm3_position_y, 39, 28); 
   SyndDataHandle(row, rm3_position_z, 50, 29); 
   SyndDataHandle(row, rm3_scale_x, 2, 30); 
   SyndDataHandle(row, rm3_scale_y, 2, 31); 
   SyndDataHandle(row, rm3_scale_z, 2, 32); 
   SyndDataHandle(row, rm3_rotation_x, 50, 33); 
   SyndDataHandle(row, rm3_rotation_y, 49, 34); 
   SyndDataHandle(row, rm3_rotation_z, 49, 35); 
   SyndDataHandle(row, rm4_position_x, 50, 36); 
   SyndDataHandle(row, rm4_position_y, 34, 37); 
   SyndDataHandle(row, rm4_position_z, 48, 38); 
   SyndDataHandle(row, rm4_scale_x, 1, 39); 
   SyndDataHandle(row, rm4_scale_y, 1, 40); 
   SyndDataHandle(row, rm4_scale_z, 1, 41); 
   SyndDataHandle(row, rm4_rotation_x, 41, 42); 
   SyndDataHandle(row, rm4_rotation_y, 44, 43); 
   SyndDataHandle(row, rm4_rotation_z, 43, 44); 
   SyndDataHandle(row, rm5_position_x, 45, 45); 
   SyndDataHandle(row, rm5_position_y, 35, 46); 
   SyndDataHandle(row, rm5_position_z, 46, 47); 
   SyndDataHandle(row, rm5_scale_x, 1, 48); 
   SyndDataHandle(row, rm5_scale_y, 1, 49); 
   SyndDataHandle(row, rm5_scale_z, 1, 50); 
   SyndDataHandle(row, rm5_rotation_x, 45, 51); 
   SyndDataHandle(row, rm5_rotation_y, 44, 52); 
   SyndDataHandle(row, rm5_rotation_z, 47, 53); 
   SyndDataHandle(row, rm6_position_x, 51, 54); 
   SyndDataHandle(row, rm6_position_y, 41, 55); 
   SyndDataHandle(row, rm6_position_z, 46, 56); 
   SyndDataHandle(row, rm6_scale_x, 1, 57); 
   SyndDataHandle(row, rm6_scale_y, 1, 58); 
   SyndDataHandle(row, rm6_scale_z, 1, 59); 
   SyndDataHandle(row, rm6_rotation_x, 47, 60); 
   SyndDataHandle(row, rm6_rotation_y, 43, 61); 
   SyndDataHandle(row, rm6_rotation_z, 46, 62); 
   SyndDataHandle(row, rm7_position_x, 45, 63); 
   SyndDataHandle(row, rm7_position_y, 32, 64); 
   SyndDataHandle(row, rm7_position_z, 47, 65); 
   SyndDataHandle(row, rm7_scale_x, 1, 66); 
   SyndDataHandle(row, rm7_scale_y, 1, 67); 
   SyndDataHandle(row, rm7_scale_z, 1, 68); 
   SyndDataHandle(row, rm7_rotation_x, 42, 69); 
   SyndDataHandle(row, rm7_rotation_y, 42, 70); 
   SyndDataHandle(row, rm7_rotation_z, 43, 71); 
   SyndDataHandle(row, rm8_position_x, 46, 72); 
   SyndDataHandle(row, rm8_position_y, 38, 73); 
   SyndDataHandle(row, rm8_position_z, 47, 74); 
   SyndDataHandle(row, rm8_scale_x, 1, 75); 
   SyndDataHandle(row, rm8_scale_y, 1, 76); 
   SyndDataHandle(row, rm8_scale_z, 1, 77); 
   SyndDataHandle(row, rm8_rotation_x, 47, 78); 
   SyndDataHandle(row, rm8_rotation_y, 44, 79); 
   SyndDataHandle(row, rm8_rotation_z, 45, 80); 
   SyndDataHandle(row, rm9_position_x, 48, 81); 
   SyndDataHandle(row, rm9_position_y, 35, 82); 
   SyndDataHandle(row, rm9_position_z, 48, 83); 
   SyndDataHandle(row, rm9_scale_x, 1, 84); 
   SyndDataHandle(row, rm9_scale_y, 1, 85); 
   SyndDataHandle(row, rm9_scale_z, 1, 86); 
   SyndDataHandle(row, rm9_rotation_x, 47, 87); 
   SyndDataHandle(row, rm9_rotation_y, 47, 88); 
   SyndDataHandle(row, rm9_rotation_z, 45, 89); 
   SyndDataHandle(row, rm10_position_x, 47, 90); 
   SyndDataHandle(row, rm10_position_y, 43, 91); 
   SyndDataHandle(row, rm10_position_z, 48, 92); 
   SyndDataHandle(row, rm10_scale_x, 1, 93); 
   SyndDataHandle(row, rm10_scale_y, 1, 94); 
   SyndDataHandle(row, rm10_scale_z, 1, 95); 
   SyndDataHandle(row, rm10_rotation_x, 47, 96); 
   SyndDataHandle(row, rm10_rotation_y, 47, 97); 
   SyndDataHandle(row, rm10_rotation_z, 48, 98); 
   SyndDataHandle(row, rm11_position_x, 51, 99); 
   SyndDataHandle(row, rm11_position_y, 46, 100); 
   SyndDataHandle(row, rm11_position_z, 51, 101); 
   SyndDataHandle(row, rm11_scale_x, 1, 102); 
   SyndDataHandle(row, rm11_scale_y, 1, 103); 
   SyndDataHandle(row, rm11_scale_z, 1, 104); 
   SyndDataHandle(row, rm11_rotation_x, 49, 105); 
   SyndDataHandle(row, rm11_rotation_y, 46, 106); 
   SyndDataHandle(row, rm11_rotation_z, 51, 107); 
} 


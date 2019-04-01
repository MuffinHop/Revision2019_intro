
    varying float RM_Objects[108];
       vec3 rm0_position_x[10] = vec3[10]( 
               vec3( 1, -3.812066, 1 ), 
               vec3( 1, -3.900717, 1 ), 
               vec3( 1, -1.5, 1 ), 
               vec3( 2, -5.63659, 1 ), 
               vec3( 2, -5.647483, 1 ), 
               vec3( 3, -7.019884, 1 ), 
               vec3( 3, -7.092824, 1 ), 
               vec3( 4, -7.617208, 1 ), 
               vec3( 4, -7.691231, 1 ), 
               vec3( 5, -7.609781, 1 ) 
              ); 
       vec3 rm0_position_y[10] = vec3[10]( 
               vec3( 1, 0.5150018, 1 ), 
               vec3( 1, 0.7378003, 1 ), 
               vec3( 1, 0.18, 1 ), 
               vec3( 2, 0.5305436, 1 ), 
               vec3( 2, 0.7604082, 1 ), 
               vec3( 3, 0.4999026, 1 ), 
               vec3( 3, 0.7131416, 1 ), 
               vec3( 4, 0.5237213, 1 ), 
               vec3( 5, 0.7288901, 1 ), 
               vec3( 6, 0.7469869, 1 ) 
              ); 
       vec3 rm0_position_z[13] = vec3[13]( 
               vec3( 1, 3.30802, 1 ), 
               vec3( 1, 3.177617, 1 ), 
               vec3( 1, 7.82, 1 ), 
               vec3( 2, 2.723551, 1 ), 
               vec3( 2, 2.570016, 1 ), 
               vec3( 3, 2.386928, 1 ), 
               vec3( 3, 2.235398, 1 ), 
               vec3( 4, 2.30084, 1 ), 
               vec3( 4, 2.137944, 1 ), 
               vec3( 5, 2.256976, 1 ), 
               vec3( 5, 2.098757, 1 ), 
               vec3( 6, 2.21146, 1 ), 
               vec3( 6, 2.072294, 1 ) 
              ); 
       vec3 rm0_scale_x[3] = vec3[3]( 
               vec3( 1, 0.2, 1 ), 
               vec3( 1, 0.22, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm0_scale_y[3] = vec3[3]( 
               vec3( 1, 0.2, 1 ), 
               vec3( 1, 0.22, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm0_scale_z[3] = vec3[3]( 
               vec3( 1, 0.2000001, 1 ), 
               vec3( 1, 0.22, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm0_rotation_x[12] = vec3[12]( 
               vec3( 1, -0.5113481, 1 ), 
               vec3( 1, 0.1945939, 1 ), 
               vec3( 2, -0.5423412, 1 ), 
               vec3( 2, 0.1573812, 1 ), 
               vec3( 3, -0.5038052, 1 ), 
               vec3( 3, 0.2354196, 1 ), 
               vec3( 4, -0.5833132, 1 ), 
               vec3( 4, 0.390653, 1 ), 
               vec3( 5, -0.6663063, 1 ), 
               vec3( 6, -0.6397372, 1 ), 
               vec3( 6, 0.2839034, 1 ), 
               vec3( 7, 0.2623226, 1 ) 
              ); 
       vec3 rm0_rotation_y[8] = vec3[8]( 
               vec3( 1, 0.3918921, 1 ), 
               vec3( 1, -0.8767608, 1 ), 
               vec3( 2, 0.6519426, 1 ), 
               vec3( 2, -0.9472041, 1 ), 
               vec3( 3, 0.5699544, 1 ), 
               vec3( 3, -0.9102961, 1 ), 
               vec3( 5, 0.6117902, 1 ), 
               vec3( 6, -0.9550033, 1 ) 
              ); 
       vec3 rm0_rotation_z[14] = vec3[14]( 
               vec3( 1, 0.4490843, 1 ), 
               vec3( 1, -0.1339544, 1 ), 
               vec3( 1, -0.3826834, 1 ), 
               vec3( 2, 0.3591691, 1 ), 
               vec3( 2, -0.1058328, 1 ), 
               vec3( 3, -0.09123299, 1 ), 
               vec3( 4, 0.4203959, 1 ), 
               vec3( 4, -0.06958453, 1 ), 
               vec3( 5, 0.3759921, 1 ), 
               vec3( 5, -0.02363826, 1 ), 
               vec3( 6, 0.4368323, 1 ), 
               vec3( 6, -0.08578059, 1 ), 
               vec3( 7, 0.4516304, 1 ), 
               vec3( 7, -0.10031, 1 ) 
              ); 
       vec3 rm1_position_x[14] = vec3[14]( 
               vec3( 1, -3.758191, 1 ), 
               vec3( 1, 0.79, 1 ), 
               vec3( 1, -3.931287, 1 ), 
               vec3( 2, -5.331381, 1 ), 
               vec3( 2, -5.684079, 1 ), 
               vec3( 3, -6.780745, 1 ), 
               vec3( 3, -7.115269, 1 ), 
               vec3( 4, -7.397518, 1 ), 
               vec3( 4, -7.68099, 1 ), 
               vec3( 5, -7.350608, 1 ), 
               vec3( 5, -7.584979, 1 ), 
               vec3( 6, -7.370039, 1 ), 
               vec3( 6, -7.613473, 1 ), 
               vec3( 7, -7.382253, 1 ) 
              ); 
       vec3 rm1_position_y[11] = vec3[11]( 
               vec3( 1, 0.2531007, 1 ), 
               vec3( 1, -1.82, 1 ), 
               vec3( 1, 0.6504244, 1 ), 
               vec3( 2, 0.4025666, 1 ), 
               vec3( 2, 0.6764543, 1 ), 
               vec3( 3, 0.3001448, 1 ), 
               vec3( 3, 0.6237715, 1 ), 
               vec3( 4, 0.6346388, 1 ), 
               vec3( 5, 0.274971, 1 ), 
               vec3( 5, 0.6447462, 1 ), 
               vec3( 6, 0.6615804, 1 ) 
              ); 
       vec3 rm1_position_z[14] = vec3[14]( 
               vec3( 1, 3.010716, 1 ), 
               vec3( 1, 5.88, 1 ), 
               vec3( 1, 3.171066, 1 ), 
               vec3( 2, 2.596246, 1 ), 
               vec3( 2, 2.555031, 1 ), 
               vec3( 3, 2.162274, 1 ), 
               vec3( 3, 2.224385, 1 ), 
               vec3( 4, 2.041172, 1 ), 
               vec3( 4, 2.108868, 1 ), 
               vec3( 5, 2.070326, 1 ), 
               vec3( 5, 2.068481, 1 ), 
               vec3( 6, 2.03829, 1 ), 
               vec3( 6, 2.036444, 1 ), 
               vec3( 7, 2.027985, 1 ) 
              ); 
       vec3 rm1_scale_x[3] = vec3[3]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ), 
               vec3( 1, 0.12, 1 ) 
              ); 
       vec3 rm1_scale_y[3] = vec3[3]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ), 
               vec3( 1, 0.12, 1 ) 
              ); 
       vec3 rm1_scale_z[3] = vec3[3]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ), 
               vec3( 1, 0.12, 1 ) 
              ); 
       vec3 rm1_rotation_x[14] = vec3[14]( 
               vec3( 1, -0.748306, 1 ), 
               vec3( 1, -0.103846, 1 ), 
               vec3( 1, 0.1945939, 1 ), 
               vec3( 2, -0.5275109, 1 ), 
               vec3( 2, 0.1573812, 1 ), 
               vec3( 3, -0.6322064, 1 ), 
               vec3( 3, 0.2354196, 1 ), 
               vec3( 4, -0.3811689, 1 ), 
               vec3( 4, 0.390653, 1 ), 
               vec3( 5, -0.4764318, 1 ), 
               vec3( 6, -0.4937638, 1 ), 
               vec3( 6, 0.2839034, 1 ), 
               vec3( 7, -0.46246, 1 ), 
               vec3( 7, 0.2623226, 1 ) 
              ); 
       vec3 rm1_rotation_y[10] = vec3[10]( 
               vec3( 1, 0.01666105, 1 ), 
               vec3( 1, -0.3132005, 1 ), 
               vec3( 1, -0.8767608, 1 ), 
               vec3( 2, -0.7640073, 1 ), 
               vec3( 2, -0.9472041, 1 ), 
               vec3( 3, -0.5311067, 1 ), 
               vec3( 3, -0.9102961, 1 ), 
               vec3( 4, -0.8288209, 1 ), 
               vec3( 5, -0.8122191, 1 ), 
               vec3( 6, -0.9550033, 1 ) 
              ); 
       vec3 rm1_rotation_z[15] = vec3[15]( 
               vec3( 1, -0.04833826, 1 ), 
               vec3( 1, -0.1669408, 1 ), 
               vec3( 1, -0.1339544, 1 ), 
               vec3( 2, 0.05832273, 1 ), 
               vec3( 2, -0.1058328, 1 ), 
               vec3( 3, -0.01621532, 1 ), 
               vec3( 3, -0.09123299, 1 ), 
               vec3( 4, 0.1601423, 1 ), 
               vec3( 4, -0.06958453, 1 ), 
               vec3( 5, 0.01725703, 1 ), 
               vec3( 5, -0.02363826, 1 ), 
               vec3( 6, 0.08416186, 1 ), 
               vec3( 6, -0.08578059, 1 ), 
               vec3( 7, 0.1402462, 1 ), 
               vec3( 7, -0.10031, 1 ) 
              ); 
       vec3 rm2_position_x[7] = vec3[7]( 
               vec3( 1, -3.60561, 1 ), 
               vec3( 1, 0.43, 1 ), 
               vec3( 2, -5.565049, 1 ), 
               vec3( 3, -6.876409, 1 ), 
               vec3( 4, -7.511654, 1 ), 
               vec3( 5, -7.579156, 1 ), 
               vec3( 6, -7.611843, 1 ) 
              ); 
       vec3 rm2_position_y[6] = vec3[6]( 
               vec3( 1, 0.3855215, 1 ), 
               vec3( 1, 0.1, 1 ), 
               vec3( 2, 0.3550711, 1 ), 
               vec3( 4, 0.3323604, 1 ), 
               vec3( 5, 0.2988392, 1 ), 
               vec3( 6, 0.2827796, 1 ) 
              ); 
       vec3 rm2_position_z[7] = vec3[7]( 
               vec3( 1, 3.379799, 1 ), 
               vec3( 1, 10.96, 1 ), 
               vec3( 2, 2.892762, 1 ), 
               vec3( 3, 2.546889, 1 ), 
               vec3( 4, 2.430379, 1 ), 
               vec3( 5, 2.37067, 1 ), 
               vec3( 6, 2.288569, 1 ) 
              ); 
       vec3 rm2_scale_x[2] = vec3[2]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm2_scale_y[2] = vec3[2]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm2_scale_z[2] = vec3[2]( 
               vec3( 1, 0.1, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm2_rotation_x[6] = vec3[6]( 
               vec3( 1, -0.2041036, 1 ), 
               vec3( 1, -0.103846, 1 ), 
               vec3( 2, -0.2565714, 1 ), 
               vec3( 4, -0.2751015, 1 ), 
               vec3( 5, -0.401461, 1 ), 
               vec3( 6, -0.4252684, 1 ) 
              ); 
       vec3 rm2_rotation_y[7] = vec3[7]( 
               vec3( 1, 0.6085035, 1 ), 
               vec3( 1, -0.3132005, 1 ), 
               vec3( 2, 0.5319643, 1 ), 
               vec3( 3, 0.6318757, 1 ), 
               vec3( 4, 0.606244, 1 ), 
               vec3( 5, 0.5742571, 1 ), 
               vec3( 6, 0.6099722, 1 ) 
              ); 
       vec3 rm2_rotation_z[7] = vec3[7]( 
               vec3( 1, 0.7540445, 1 ), 
               vec3( 1, -0.1669408, 1 ), 
               vec3( 2, 0.6005924, 1 ), 
               vec3( 3, 0.6462141, 1 ), 
               vec3( 4, 0.6942236, 1 ), 
               vec3( 5, 0.6591371, 1 ), 
               vec3( 6, 0.630108, 1 ) 
              ); 
       vec3 rm3_position_x[7] = vec3[7]( 
               vec3( 1, -3.804824, 1 ), 
               vec3( 1, 2.465, 1 ), 
               vec3( 2, -5.533244, 1 ), 
               vec3( 3, -6.95949, 1 ), 
               vec3( 4, -7.501342, 1 ), 
               vec3( 5, -7.414055, 1 ), 
               vec3( 6, -7.438118, 1 ) 
              ); 
       vec3 rm3_position_y[5] = vec3[5]( 
               vec3( 1, 0.4994681, 1 ), 
               vec3( 1, 0.18, 1 ), 
               vec3( 2, 0.5132941, 1 ), 
               vec3( 3, 0.4790334, 1 ), 
               vec3( 4, 0.520779, 1 ) 
              ); 
       vec3 rm3_position_z[7] = vec3[7]( 
               vec3( 1, 3.051576, 1 ), 
               vec3( 1, 10.74, 1 ), 
               vec3( 2, 2.488861, 1 ), 
               vec3( 3, 2.137981, 1 ), 
               vec3( 4, 2.071441, 1 ), 
               vec3( 5, 2.104304, 1 ), 
               vec3( 6, 2.036363, 1 ) 
              ); 
       vec3 rm3_scale_x[2] = vec3[2]( 
               vec3( 1, 0.2000001, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm3_scale_y[2] = vec3[2]( 
               vec3( 1, 0.2, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm3_scale_z[2] = vec3[2]( 
               vec3( 1, 0.2000001, 1 ), 
               vec3( 1, 1, 1 ) 
              ); 
       vec3 rm3_rotation_x[8] = vec3[8]( 
               vec3( 1, -0.1160909, 1 ), 
               vec3( 1, -0.2482672, 1 ), 
               vec3( 2, 0.006561905, 1 ), 
               vec3( 3, -0.03390123, 1 ), 
               vec3( 4, -0.1841778, 1 ), 
               vec3( 5, -0.324394, 1 ), 
               vec3( 6, -0.3093309, 1 ), 
               vec3( 7, -0.3203369, 1 ) 
              ); 
       vec3 rm3_rotation_y[5] = vec3[5]( 
               vec3( 1, -0.7560883, 1 ), 
               vec3( 2, -0.8079019, 1 ), 
               vec3( 3, -0.8482867, 1 ), 
               vec3( 4, -0.8924837, 1 ), 
               vec3( 6, -0.8754758, 1 ) 
              ); 
       vec3 rm3_rotation_z[6] = vec3[6]( 
               vec3( 1, 0.4913921, 1 ), 
               vec3( 2, 0.5560193, 1 ), 
               vec3( 3, 0.5272199, 1 ), 
               vec3( 4, 0.3917476, 1 ), 
               vec3( 5, 0.2898165, 1 ), 
               vec3( 6, 0.3694972, 1 ) 
              ); 
       vec3 rm4_position_x[5] = vec3[5]( 
               vec3( 1, -3.755026, 1 ), 
               vec3( 2, -5.547758, 1 ), 
               vec3( 3, -6.904153, 1 ), 
               vec3( 4, -7.49389, 1 ), 
               vec3( 6, -7.516315, 1 ) 
              ); 
       vec3 rm4_position_y[5] = vec3[5]( 
               vec3( 1, 0.2538971, 1 ), 
               vec3( 2, 0.2664548, 1 ), 
               vec3( 3, 0.2383403, 1 ), 
               vec3( 4, 0.2700117, 1 ), 
               vec3( 6, 0.2592641, 1 ) 
              ); 
       vec3 rm4_position_z[6] = vec3[6]( 
               vec3( 1, 3.209789, 1 ), 
               vec3( 2, 2.637747, 1 ), 
               vec3( 3, 2.310942, 1 ), 
               vec3( 4, 2.214324, 1 ), 
               vec3( 5, 2.186601, 1 ), 
               vec3( 6, 2.153123, 1 ) 
              ); 
       vec3 rm4_scale_x[1] = vec3[1]( 
               vec3( 1, 0.24, 1 ) 
              ); 
       vec3 rm4_scale_y[1] = vec3[1]( 
               vec3( 1, 0.24, 1 ) 
              ); 
       vec3 rm4_scale_z[1] = vec3[1]( 
               vec3( 1, 0.24, 1 ) 
              ); 
       vec3 rm4_rotation_x[6] = vec3[6]( 
               vec3( 1, 0.082214, 1 ), 
               vec3( 2, 0.05589017, 1 ), 
               vec3( 3, -0.05229412, 1 ), 
               vec3( 4, 0.05424643, 1 ), 
               vec3( 5, 0.06823315, 1 ), 
               vec3( 6, 0.0946942, 1 ) 
              ); 
       vec3 rm4_rotation_y[5] = vec3[5]( 
               vec3( 1, -0.8952451, 1 ), 
               vec3( 2, -0.9512162, 1 ), 
               vec3( 3, -0.9295025, 1 ), 
               vec3( 4, -0.9402781, 1 ), 
               vec3( 5, -0.9659711, 1 ) 
              ); 
       vec3 rm4_rotation_z[5] = vec3[5]( 
               vec3( 1, -0.1998252, 1 ), 
               vec3( 2, -0.1535004, 1 ), 
               vec3( 3, -0.2382928, 1 ), 
               vec3( 4, -0.189411, 1 ), 
               vec3( 6, -0.1740857, 1 ) 
              ); 
       vec3 rm5_position_x[6] = vec3[6]( 
               vec3( 1, -3.892755, 1 ), 
               vec3( 2, -5.160574, 1 ), 
               vec3( 3, -7.059129, 1 ), 
               vec3( 4, -7.506156, 1 ), 
               vec3( 5, -7.48563, 1 ), 
               vec3( 7, -7.475335, 1 ) 
              ); 
       vec3 rm5_position_y[5] = vec3[5]( 
               vec3( 1, -0.7045382, 1 ), 
               vec3( 2, -0.6184784, 1 ), 
               vec3( 3, -0.6576408, 1 ), 
               vec3( 4, -0.7427563, 1 ), 
               vec3( 6, -0.7569156, 1 ) 
              ); 
       vec3 rm5_position_z[6] = vec3[6]( 
               vec3( 1, 3.272482, 1 ), 
               vec3( 2, 2.798091, 1 ), 
               vec3( 3, 2.336997, 1 ), 
               vec3( 4, 2.240144, 1 ), 
               vec3( 5, 2.258508, 1 ), 
               vec3( 6, 2.227803, 1 ) 
              ); 
       vec3 rm5_scale_x[1] = vec3[1]( 
               vec3( 1, 0.3, 1 ) 
              ); 
       vec3 rm5_scale_y[1] = vec3[1]( 
               vec3( 1, 0.3000001, 1 ) 
              ); 
       vec3 rm5_scale_z[1] = vec3[1]( 
               vec3( 1, 0.3, 1 ) 
              ); 
       vec3 rm5_rotation_x[6] = vec3[6]( 
               vec3( 1, -0.04306927, 1 ), 
               vec3( 2, 0.301813, 1 ), 
               vec3( 3, 0.2173546, 1 ), 
               vec3( 4, -0.01223303, 1 ), 
               vec3( 5, -0.1055116, 1 ), 
               vec3( 6, -0.0276574, 1 ) 
              ); 
       vec3 rm5_rotation_y[6] = vec3[6]( 
               vec3( 1, -0.8589826, 1 ), 
               vec3( 2, -0.8117666, 1 ), 
               vec3( 3, -0.8540232, 1 ), 
               vec3( 4, -0.9092798, 1 ), 
               vec3( 5, -0.9267588, 1 ), 
               vec3( 6, -0.952705, 1 ) 
              ); 
       vec3 rm5_rotation_z[5] = vec3[5]( 
               vec3( 1, -0.1837313, 1 ), 
               vec3( 2, 0.06894824, 1 ), 
               vec3( 3, -0.0669597, 1 ), 
               vec3( 4, -0.208035, 1 ), 
               vec3( 5, -0.240115, 1 ) 
              ); 
       vec3 rm6_position_x[6] = vec3[6]( 
               vec3( 1, -3.819234, 1 ), 
               vec3( 2, -5.580227, 1 ), 
               vec3( 3, -6.992264, 1 ), 
               vec3( 4, -7.552688, 1 ), 
               vec3( 5, -7.493419, 1 ), 
               vec3( 6, -7.511835, 1 ) 
              ); 
       vec3 rm6_position_y[4] = vec3[4]( 
               vec3( 1, 0.5075064, 1 ), 
               vec3( 2, 0.5223222, 1 ), 
               vec3( 3, 0.488564, 1 ), 
               vec3( 4, 0.5239388, 1 ) 
              ); 
       vec3 rm6_position_z[5] = vec3[5]( 
               vec3( 1, 3.141106, 1 ), 
               vec3( 2, 2.566313, 1 ), 
               vec3( 3, 2.222378, 1 ), 
               vec3( 4, 2.146551, 1 ), 
               vec3( 6, 2.089164, 1 ) 
              ); 
       vec3 rm6_scale_x[1] = vec3[1]( 
               vec3( 1, 0.2, 1 ) 
              ); 
       vec3 rm6_scale_y[1] = vec3[1]( 
               vec3( 1, 0.2, 1 ) 
              ); 
       vec3 rm6_scale_z[1] = vec3[1]( 
               vec3( 1, 0.2, 1 ) 
              ); 
       vec3 rm6_rotation_x[5] = vec3[5]( 
               vec3( 1, 0.1228314, 1 ), 
               vec3( 3, 0.1413831, 1 ), 
               vec3( 4, 0.175636, 1 ), 
               vec3( 5, 0.1481564, 1 ), 
               vec3( 7, 0.1381039, 1 ) 
              ); 
       vec3 rm6_rotation_y[5] = vec3[5]( 
               vec3( 1, -0.8795367, 1 ), 
               vec3( 2, -0.9506904, 1 ), 
               vec3( 3, -0.9203743, 1 ), 
               vec3( 4, -0.9493453, 1 ), 
               vec3( 5, -0.982861, 1 ) 
              ); 
       vec3 rm6_rotation_z[5] = vec3[5]( 
               vec3( 1, -0.1175032, 1 ), 
               vec3( 2, -0.09930874, 1 ), 
               vec3( 3, -0.08685457, 1 ), 
               vec3( 5, -0.1096806, 1 ), 
               vec3( 6, -0.0877201, 1 ) 
              ); 
       vec3 rm7_position_x[5] = vec3[5]( 
               vec3( 1, -3.323279, 1 ), 
               vec3( 2, -5.738527, 1 ), 
               vec3( 3, -7.280276, 1 ), 
               vec3( 4, -7.455974, 1 ), 
               vec3( 6, -7.416827, 1 ) 
              ); 
       vec3 rm7_position_y[5] = vec3[5]( 
               vec3( 1, -0.6620663, 1 ), 
               vec3( 2, -0.7187793, 1 ), 
               vec3( 3, -0.6807071, 1 ), 
               vec3( 4, -0.7490962, 1 ), 
               vec3( 5, -0.759917, 1 ) 
              ); 
       vec3 rm7_position_z[6] = vec3[6]( 
               vec3( 1, 3.170134, 1 ), 
               vec3( 2, 2.515562, 1 ), 
               vec3( 3, 2.120761, 1 ), 
               vec3( 4, 2.095258, 1 ), 
               vec3( 5, 2.107746, 1 ), 
               vec3( 6, 2.126694, 1 ) 
              ); 
       vec3 rm7_scale_x[1] = vec3[1]( 
               vec3( 1, 0.3, 1 ) 
              ); 
       vec3 rm7_scale_y[1] = vec3[1]( 
               vec3( 1, 0.3, 1 ) 
              ); 
       vec3 rm7_scale_z[1] = vec3[1]( 
               vec3( 1, 0.3, 1 ) 
              ); 
       vec3 rm7_rotation_x[6] = vec3[6]( 
               vec3( 1, 0.3616988, 1 ), 
               vec3( 2, -0.04690062, 1 ), 
               vec3( 3, -0.09854913, 1 ), 
               vec3( 4, 0.08541288, 1 ), 
               vec3( 5, 0.06265805, 1 ), 
               vec3( 6, 0.1079007, 1 ) 
              ); 
       vec3 rm7_rotation_y[4] = vec3[4]( 
               vec3( 1, -0.7909352, 1 ), 
               vec3( 2, -0.9131687, 1 ), 
               vec3( 5, -0.9282655, 1 ), 
               vec3( 6, -0.9636776, 1 ) 
              ); 
       vec3 rm7_rotation_z[6] = vec3[6]( 
               vec3( 1, 0.02106054, 1 ), 
               vec3( 2, -0.2807904, 1 ), 
               vec3( 3, -0.3212612, 1 ), 
               vec3( 4, -0.2367379, 1 ), 
               vec3( 5, -0.2126369, 1 ), 
               vec3( 7, -0.2263817, 1 ) 
              ); 
       vec3 rm8_position_x[6] = vec3[6]( 
               vec3( 1, -3.984429, 1 ), 
               vec3( 2, -5.4828, 1 ), 
               vec3( 3, -7.241667, 1 ), 
               vec3( 4, -7.617958, 1 ), 
               vec3( 5, -7.600911, 1 ), 
               vec3( 6, -7.589364, 1 ) 
              ); 
       vec3 rm8_position_y[5] = vec3[5]( 
               vec3( 1, -0.2957186, 1 ), 
               vec3( 2, -0.3573811, 1 ), 
               vec3( 3, -0.2769406, 1 ), 
               vec3( 4, -0.3408777, 1 ), 
               vec3( 5, -0.3595733, 1 ) 
              ); 
       vec3 rm8_position_z[6] = vec3[6]( 
               vec3( 1, 3.245247, 1 ), 
               vec3( 2, 2.721384, 1 ), 
               vec3( 3, 2.309822, 1 ), 
               vec3( 4, 2.199418, 1 ), 
               vec3( 5, 2.173307, 1 ), 
               vec3( 6, 2.134996, 1 ) 
              ); 
       vec3 rm8_scale_x[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm8_scale_y[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm8_scale_z[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm8_rotation_x[6] = vec3[6]( 
               vec3( 1, 0.13274, 1 ), 
               vec3( 2, 0.4070124, 1 ), 
               vec3( 3, 0.2460864, 1 ), 
               vec3( 4, 0.1951057, 1 ), 
               vec3( 5, 0.2090228, 1 ), 
               vec3( 6, 0.2322819, 1 ) 
              ); 
       vec3 rm8_rotation_y[6] = vec3[6]( 
               vec3( 1, -0.8808942, 1 ), 
               vec3( 2, -0.8124876, 1 ), 
               vec3( 3, -0.8754351, 1 ), 
               vec3( 4, -0.931056, 1 ), 
               vec3( 5, -0.9511442, 1 ), 
               vec3( 6, -0.9619976, 1 ) 
              ); 
       vec3 rm8_rotation_z[5] = vec3[5]( 
               vec3( 1, -0.1230018, 1 ), 
               vec3( 2, 0.06805776, 1 ), 
               vec3( 3, -0.07839534, 1 ), 
               vec3( 4, -0.105213, 1 ), 
               vec3( 5, -0.05421551, 1 ) 
              ); 
       vec3 rm9_position_x[5] = vec3[5]( 
               vec3( 1, -3.628058, 1 ), 
               vec3( 2, -5.742657, 1 ), 
               vec3( 3, -7.219864, 1 ), 
               vec3( 4, -7.586241, 1 ), 
               vec3( 6, -7.551856, 1 ) 
              ); 
       vec3 rm9_position_y[5] = vec3[5]( 
               vec3( 1, -0.3688095, 1 ), 
               vec3( 2, -0.2960145, 1 ), 
               vec3( 3, -0.2640738, 1 ), 
               vec3( 4, -0.3466943, 1 ), 
               vec3( 6, -0.3656873, 1 ) 
              ); 
       vec3 rm9_position_z[6] = vec3[6]( 
               vec3( 1, 3.151619, 1 ), 
               vec3( 2, 2.537587, 1 ), 
               vec3( 3, 2.165474, 1 ), 
               vec3( 4, 2.113572, 1 ), 
               vec3( 5, 2.093118, 1 ), 
               vec3( 6, 2.066943, 1 ) 
              ); 
       vec3 rm9_scale_x[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm9_scale_y[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm9_scale_z[1] = vec3[1]( 
               vec3( 1, 0.22, 1 ) 
              ); 
       vec3 rm9_rotation_x[6] = vec3[6]( 
               vec3( 1, 0.3471078, 1 ), 
               vec3( 2, 0.05850668, 1 ), 
               vec3( 3, 0.0006908029, 1 ), 
               vec3( 4, 0.1914295, 1 ), 
               vec3( 5, 0.1759528, 1 ), 
               vec3( 6, 0.2172226, 1 ) 
              ); 
       vec3 rm9_rotation_y[5] = vec3[5]( 
               vec3( 1, -0.7900338, 1 ), 
               vec3( 2, -0.9063044, 1 ), 
               vec3( 4, -0.8853801, 1 ), 
               vec3( 5, -0.9079642, 1 ), 
               vec3( 6, -0.9459372, 1 ) 
              ); 
       vec3 rm9_rotation_z[6] = vec3[6]( 
               vec3( 1, 0.02498774, 1 ), 
               vec3( 2, -0.2112391, 1 ), 
               vec3( 3, -0.2609312, 1 ), 
               vec3( 4, -0.1493505, 1 ), 
               vec3( 5, -0.121682, 1 ), 
               vec3( 6, -0.07495578, 1 ) 
              ); 
       vec3 rm10_position_x[6] = vec3[6]( 
               vec3( 1, -3.840082, 1 ), 
               vec3( 2, -5.800452, 1 ), 
               vec3( 3, -7.139257, 1 ), 
               vec3( 4, -7.756196, 1 ), 
               vec3( 5, -7.77113, 1 ), 
               vec3( 6, -7.793207, 1 ) 
              ); 
       vec3 rm10_position_y[6] = vec3[6]( 
               vec3( 1, 0.1751458, 1 ), 
               vec3( 2, 0.2971051, 1 ), 
               vec3( 3, 0.2183986, 1 ), 
               vec3( 4, 0.1520158, 1 ), 
               vec3( 5, 0.07801372, 1 ), 
               vec3( 6, 0.03887475, 1 ) 
              ); 
       vec3 rm10_position_z[6] = vec3[6]( 
               vec3( 1, 3.357782, 1 ), 
               vec3( 2, 2.690365, 1 ), 
               vec3( 3, 2.451143, 1 ), 
               vec3( 4, 2.346096, 1 ), 
               vec3( 5, 2.252868, 1 ), 
               vec3( 6, 2.20134, 1 ) 
              ); 
       vec3 rm10_scale_x[1] = vec3[1]( 
               vec3( 1, 0.1000001, 1 ) 
              ); 
       vec3 rm10_scale_y[1] = vec3[1]( 
               vec3( 1, 0.1, 1 ) 
              ); 
       vec3 rm10_scale_z[1] = vec3[1]( 
               vec3( 1, 0.1000001, 1 ) 
              ); 
       vec3 rm10_rotation_x[5] = vec3[5]( 
               vec3( 1, 0.3222997, 1 ), 
               vec3( 2, 0.389182, 1 ), 
               vec3( 3, 0.3663027, 1 ), 
               vec3( 5, 0.4960798, 1 ), 
               vec3( 6, 0.5587291, 1 ) 
              ); 
       vec3 rm10_rotation_y[6] = vec3[6]( 
               vec3( 1, -0.5494596, 1 ), 
               vec3( 2, -0.5354119, 1 ), 
               vec3( 3, -0.6025564, 1 ), 
               vec3( 4, -0.5641785, 1 ), 
               vec3( 5, -0.5366178, 1 ), 
               vec3( 6, -0.5620292, 1 ) 
              ); 
       vec3 rm10_rotation_z[6] = vec3[6]( 
               vec3( 1, -0.76851, 1 ), 
               vec3( 2, -0.5942432, 1 ), 
               vec3( 3, -0.6669807, 1 ), 
               vec3( 4, -0.7188157, 1 ), 
               vec3( 5, -0.6626775, 1 ), 
               vec3( 6, -0.5939969, 1 ) 
              ); 
       vec3 rm11_position_x[6] = vec3[6]( 
               vec3( 1, -3.990495, 1 ), 
               vec3( 2, -5.471137, 1 ), 
               vec3( 3, -7.056211, 1 ), 
               vec3( 4, -7.477251, 1 ), 
               vec3( 5, -7.451472, 1 ), 
               vec3( 6, -7.476398, 1 ) 
              ); 
       vec3 rm11_position_y[7] = vec3[7]( 
               vec3( 1, 0.4550551, 1 ), 
               vec3( 2, 0.1983937, 1 ), 
               vec3( 3, 0.2411156, 1 ), 
               vec3( 4, 0.0381245, 1 ), 
               vec3( 5, 0.07267386, 1 ), 
               vec3( 6, 0.0342859, 1 ), 
               vec3( 7, 0.01002282, 1 ) 
              ); 
       vec3 rm11_position_z[6] = vec3[6]( 
               vec3( 1, 3.08479, 1 ), 
               vec3( 2, 2.399864, 1 ), 
               vec3( 3, 2.019379, 1 ), 
               vec3( 4, 1.867343, 1 ), 
               vec3( 5, 1.848266, 1 ), 
               vec3( 7, 1.864032, 1 ) 
              ); 
       vec3 rm11_scale_x[1] = vec3[1]( 
               vec3( 1, 0.1, 1 ) 
              ); 
       vec3 rm11_scale_y[1] = vec3[1]( 
               vec3( 1, 0.1, 1 ) 
              ); 
       vec3 rm11_scale_z[1] = vec3[1]( 
               vec3( 1, 0.1, 1 ) 
              ); 
       vec3 rm11_rotation_x[7] = vec3[7]( 
               vec3( 1, -0.6362007, 1 ), 
               vec3( 2, -0.4979045, 1 ), 
               vec3( 3, -0.5893453, 1 ), 
               vec3( 4, -0.2863683, 1 ), 
               vec3( 5, -0.405522, 1 ), 
               vec3( 6, -0.4188173, 1 ), 
               vec3( 7, -0.3753502, 1 ) 
              ); 
       vec3 rm11_rotation_y[7] = vec3[7]( 
               vec3( 1, -0.04001525, 1 ), 
               vec3( 2, -0.7820002, 1 ), 
               vec3( 3, -0.5801954, 1 ), 
               vec3( 4, -0.796528, 1 ), 
               vec3( 5, -0.8112798, 1 ), 
               vec3( 6, -0.7884986, 1 ), 
               vec3( 7, -0.7782444, 1 ) 
              ); 
       vec3 rm11_rotation_z[7] = vec3[7]( 
               vec3( 1, 0.04849902, 1 ), 
               vec3( 2, 0.2049219, 1 ), 
               vec3( 3, 0.1283887, 1 ), 
               vec3( 4, 0.3719375, 1 ), 
               vec3( 5, 0.2284676, 1 ), 
               vec3( 6, 0.2946885, 1 ), 
               vec3( 7, 0.366888, 1 ) 
              ); 
       vec3 iMouseX[1] = vec3[1]( 
               vec3( 78, 0, 0 ) 
              ); 
       vec3 iMouseY[0] = vec3[0]( 
              ); 
       vec3 iTime[0] = vec3[0]( 
              ); 
       vec3 DirectionalLightX[1] = vec3[1]( 
               vec3( 0, 0, 0 ) 
              ); 
       vec3 DirectionalLightY[1] = vec3[1]( 
               vec3( 0, -1, 0 ) 
              ); 
       vec3 DirectionalLightZ[2] = vec3[2]( 
               vec3( 0, -1, 1 ), 
               vec3( 97, -1, 0 ) 
              ); 
       vec3 DirectionalLightR[1] = vec3[1]( 
               vec3( 0, 2, 0 ) 
              ); 
       vec3 DirectionalLightG[1] = vec3[1]( 
               vec3( 0, 2, 0 ) 
              ); 
       vec3 DirectionalLightB[1] = vec3[1]( 
               vec3( 0, 2, 0 ) 
              ); 
       vec3 PointLightPositionX[2] = vec3[2]( 
               vec3( 0, 0, 0 ), 
               vec3( 140, 4, 0 ) 
              ); 
       vec3 PointLightPositionY[1] = vec3[1]( 
               vec3( 0, 1, 0 ) 
              ); 
       vec3 PointLightPositionZ[2] = vec3[2]( 
               vec3( 0, 1, 0 ), 
               vec3( 162, -2, 0 ) 
              ); 
       vec3 PointLightR[1] = vec3[1]( 
               vec3( 0, 188, 0 ) 
              ); 
       vec3 PointLightG[1] = vec3[1]( 
               vec3( 0, 122, 0 ) 
              ); 
       vec3 PointLightB[1] = vec3[1]( 
               vec3( 0, 32, 0 ) 
              ); 
       vec3 CameraPositionX[2] = vec3[2]( 
               vec3( 0, -4, 1 ), 
               vec3( 87, 0.5, 0 ) 
              ); 
       vec3 CameraPositionY[1] = vec3[1]( 
               vec3( 0, 0, 0 ) 
              ); 
       vec3 CameraPositionZ[3] = vec3[3]( 
               vec3( 0, 22, 0 ), 
               vec3( 184, -8, 1 ), 
               vec3( 533, -2, 0 ) 
              ); 
       vec3 LookAtX[3] = vec3[3]( 
               vec3( 0, -7, 1 ), 
               vec3( 87, 0.5, 0 ), 
               vec3( 184, 2, 0 ) 
              ); 
       vec3 LookAtY[1] = vec3[1]( 
               vec3( 0, 0, 0 ) 
              ); 
       vec3 LookAtZ[2] = vec3[2]( 
               vec3( 0, 2, 0 ), 
               vec3( 184, 12, 0 ) 
              ); 
       vec3 CameraUpX[1] = vec3[1]( 
               vec3( 0, 0, 0 ) 
              ); 
       vec3 CameraUpY[1] = vec3[1]( 
               vec3( 0, 1, 0 ) 
              ); 
       vec3 CameraUpZ[1] = vec3[1]( 
               vec3( 0, 0, 0 ) 
              ); 
       vec3 FOV[2] = vec3[2]( 
               vec3( 0, 11, 1 ), 
               vec3( 535, 66, 0 ) 
              ); 
       vec3 Epsilon[1] = vec3[1]( 
               vec3( 0, 0.01, 0 ) 
              ); 


	int R_INDX;
	vec3 RET;
	float fRET;
#define row x
#define value y
#define type z
#define findClosest(arr,currow) { for(int i=0; i<arr.length(); i++) { if(currow<=arr[i].row) { R_INDX = i-1; RET=arr[R_INDX]; break; } } }
#define rType(r, t) { switch (int(r)) { case 0: t = 0.; break; case 2: t = t * t * (3. - 2. * t); break; case 3: t = pow(t, 2.); break; } }
#define setVal(arr,row,target) { findClosest(arr,row); float t = (row - RET.row) / (arr[R_INDX+1].row - RET.row); rType(RET.type,t); float renVal = RET.value + (arr[R_INDX+1].value - RET.value) * t; target = renVal; }
uniform float seconds;
    void main(void)
    {
	    float div = 4.0 * 60.0 / 120.0;
	    float row = seconds * div;
       setVal(rm0_position_x, row, RM_Objects[0]); 
       setVal(rm0_position_y, row, RM_Objects[1]); 
       setVal(rm0_position_z, row, RM_Objects[2]); 
       setVal(rm0_scale_x, row, RM_Objects[3]); 
       setVal(rm0_scale_y, row, RM_Objects[4]); 
       setVal(rm0_scale_z, row, RM_Objects[5]); 
       setVal(rm0_rotation_x, row, RM_Objects[6]); 
       setVal(rm0_rotation_y, row, RM_Objects[7]); 
       setVal(rm0_rotation_z, row, RM_Objects[8]); 
       setVal(rm1_position_x, row, RM_Objects[9]); 
       setVal(rm1_position_y, row, RM_Objects[10]); 
       setVal(rm1_position_z, row, RM_Objects[11]); 
       setVal(rm1_scale_x, row, RM_Objects[12]); 
       setVal(rm1_scale_y, row, RM_Objects[13]); 
       setVal(rm1_scale_z, row, RM_Objects[14]); 
       setVal(rm1_rotation_x, row, RM_Objects[15]); 
       setVal(rm1_rotation_y, row, RM_Objects[16]); 
       setVal(rm1_rotation_z, row, RM_Objects[17]); 
       setVal(rm2_position_x, row, RM_Objects[18]); 
       setVal(rm2_position_y, row, RM_Objects[19]); 
       setVal(rm2_position_z, row, RM_Objects[20]); 
       setVal(rm2_scale_x, row, RM_Objects[21]); 
       setVal(rm2_scale_y, row, RM_Objects[22]); 
       setVal(rm2_scale_z, row, RM_Objects[23]); 
       setVal(rm2_rotation_x, row, RM_Objects[24]); 
       setVal(rm2_rotation_y, row, RM_Objects[25]); 
       setVal(rm2_rotation_z, row, RM_Objects[26]); 
       setVal(rm3_position_x, row, RM_Objects[27]); 
       setVal(rm3_position_y, row, RM_Objects[28]); 
       setVal(rm3_position_z, row, RM_Objects[29]); 
       setVal(rm3_scale_x, row, RM_Objects[30]); 
       setVal(rm3_scale_y, row, RM_Objects[31]); 
       setVal(rm3_scale_z, row, RM_Objects[32]); 
       setVal(rm3_rotation_x, row, RM_Objects[33]); 
       setVal(rm3_rotation_y, row, RM_Objects[34]); 
       setVal(rm3_rotation_z, row, RM_Objects[35]); 
       setVal(rm4_position_x, row, RM_Objects[36]); 
       setVal(rm4_position_y, row, RM_Objects[37]); 
       setVal(rm4_position_z, row, RM_Objects[38]); 
       setVal(rm4_scale_x, row, RM_Objects[39]); 
       setVal(rm4_scale_y, row, RM_Objects[40]); 
       setVal(rm4_scale_z, row, RM_Objects[41]); 
       setVal(rm4_rotation_x, row, RM_Objects[42]); 
       setVal(rm4_rotation_y, row, RM_Objects[43]); 
       setVal(rm4_rotation_z, row, RM_Objects[44]); 
       setVal(rm5_position_x, row, RM_Objects[45]); 
       setVal(rm5_position_y, row, RM_Objects[46]); 
       setVal(rm5_position_z, row, RM_Objects[47]); 
       setVal(rm5_scale_x, row, RM_Objects[48]); 
       setVal(rm5_scale_y, row, RM_Objects[49]); 
       setVal(rm5_scale_z, row, RM_Objects[50]); 
       setVal(rm5_rotation_x, row, RM_Objects[51]); 
       setVal(rm5_rotation_y, row, RM_Objects[52]); 
       setVal(rm5_rotation_z, row, RM_Objects[53]); 
       setVal(rm6_position_x, row, RM_Objects[54]); 
       setVal(rm6_position_y, row, RM_Objects[55]); 
       setVal(rm6_position_z, row, RM_Objects[56]); 
       setVal(rm6_scale_x, row, RM_Objects[57]); 
       setVal(rm6_scale_y, row, RM_Objects[58]); 
       setVal(rm6_scale_z, row, RM_Objects[59]); 
       setVal(rm6_rotation_x, row, RM_Objects[60]); 
       setVal(rm6_rotation_y, row, RM_Objects[61]); 
       setVal(rm6_rotation_z, row, RM_Objects[62]); 
       setVal(rm7_position_x, row, RM_Objects[63]); 
       setVal(rm7_position_y, row, RM_Objects[64]); 
       setVal(rm7_position_z, row, RM_Objects[65]); 
       setVal(rm7_scale_x, row, RM_Objects[66]); 
       setVal(rm7_scale_y, row, RM_Objects[67]); 
       setVal(rm7_scale_z, row, RM_Objects[68]); 
       setVal(rm7_rotation_x, row, RM_Objects[69]); 
       setVal(rm7_rotation_y, row, RM_Objects[70]); 
       setVal(rm7_rotation_z, row, RM_Objects[71]); 
       setVal(rm8_position_x, row, RM_Objects[72]); 
       setVal(rm8_position_y, row, RM_Objects[73]); 
       setVal(rm8_position_z, row, RM_Objects[74]); 
       setVal(rm8_scale_x, row, RM_Objects[75]); 
       setVal(rm8_scale_y, row, RM_Objects[76]); 
       setVal(rm8_scale_z, row, RM_Objects[77]); 
       setVal(rm8_rotation_x, row, RM_Objects[78]); 
       setVal(rm8_rotation_y, row, RM_Objects[79]); 
       setVal(rm8_rotation_z, row, RM_Objects[80]); 
       setVal(rm9_position_x, row, RM_Objects[81]); 
       setVal(rm9_position_y, row, RM_Objects[82]); 
       setVal(rm9_position_z, row, RM_Objects[83]); 
       setVal(rm9_scale_x, row, RM_Objects[84]); 
       setVal(rm9_scale_y, row, RM_Objects[85]); 
       setVal(rm9_scale_z, row, RM_Objects[86]); 
       setVal(rm9_rotation_x, row, RM_Objects[87]); 
       setVal(rm9_rotation_y, row, RM_Objects[88]); 
       setVal(rm9_rotation_z, row, RM_Objects[89]); 
       setVal(rm10_position_x, row, RM_Objects[90]); 
       setVal(rm10_position_y, row, RM_Objects[91]); 
       setVal(rm10_position_z, row, RM_Objects[92]); 
       setVal(rm10_scale_x, row, RM_Objects[93]); 
       setVal(rm10_scale_y, row, RM_Objects[94]); 
       setVal(rm10_scale_z, row, RM_Objects[95]); 
       setVal(rm10_rotation_x, row, RM_Objects[96]); 
       setVal(rm10_rotation_y, row, RM_Objects[97]); 
       setVal(rm10_rotation_z, row, RM_Objects[98]); 
       setVal(rm11_position_x, row, RM_Objects[99]); 
       setVal(rm11_position_y, row, RM_Objects[100]); 
       setVal(rm11_position_z, row, RM_Objects[101]); 
       setVal(rm11_scale_x, row, RM_Objects[102]); 
       setVal(rm11_scale_y, row, RM_Objects[103]); 
       setVal(rm11_scale_z, row, RM_Objects[104]); 
       setVal(rm11_rotation_x, row, RM_Objects[105]); 
       setVal(rm11_rotation_y, row, RM_Objects[106]); 
       setVal(rm11_rotation_z, row, RM_Objects[107]); 

       gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }

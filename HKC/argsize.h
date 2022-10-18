#ifndef ARGSIZE_H
#define ARGSIZE_H

#include <stdarg.h>

#define INDEX_RSEQ() 256, 255, 254, 253, 252, 251, 250,\
                      249, 248, 247, 246, 245, 244, 243, 242, 241, 240,\
                      239, 238, 237, 236, 235, 234, 233, 232, 231, 230,\
                      229, 228, 227, 226, 225, 224, 223, 222, 221, 220,\
                      219, 218, 217, 216, 215, 214, 213, 212, 211, 210,\
                      209, 208, 207, 206, 205, 204, 203, 202, 201, 200,\
                      199, 198, 197, 196, 195, 194, 193, 192, 191, 190,\
                      189, 188, 187, 186, 185, 184, 183, 182, 181, 180,\
                      179, 178, 177, 176, 175, 174, 173, 172, 171, 170,\
                      169, 168, 167, 166, 165, 164, 163, 162, 161, 160,\
                      159, 158, 157, 156, 155, 154, 153, 152, 151, 150,\
                      149, 148, 147, 146, 145, 144, 143, 142, 141, 140,\
                      139, 138, 137, 136, 135, 134, 133, 132, 131, 130,\
                      129, 128, 127, 126, 125, 124, 123, 122, 121, 120,\
                      119, 118, 117, 116, 115, 114, 113, 112, 111, 110,\
                      109, 108, 107, 106, 105, 104, 103, 102, 101, 100,\
                      99, 98, 97, 96, 95, 94, 93, 92, 91, 90,\
                      89, 88, 87, 86, 85, 84, 83, 82, 81, 80,\
                      79, 78, 77, 76, 75, 74, 73, 72, 71, 70,\
                      69, 68, 67, 66, 65, 64, 63, 62, 61, 60,\
                      59, 58, 57, 56, 55, 54, 53, 52, 51, 50,\
                      49, 48, 47, 46, 45, 44, 43, 42, 41, 40,\
                      39, 38, 37, 36, 35, 34, 33, 32, 31, 30,\
                      29, 28, 27, 26, 25, 24, 23, 22, 21, 20,\
                      19, 18, 17, 16, 15, 14, 13, 12, 11, 10,\
                      9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define VA_ARGS_SIZE_IMPL(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9, ARG10,\
                          ARG11, ARG12, ARG13, ARG14, ARG15, ARG16, ARG17, ARG18, ARG19, ARG20,\
                          ARG21, ARG22, ARG23, ARG24, ARG25, ARG26, ARG27, ARG28, ARG29, ARG30,\
                          ARG31, ARG32, ARG33, ARG34, ARG35, ARG36, ARG37, ARG38, ARG39, ARG40,\
                          ARG41, ARG42, ARG43, ARG44, ARG45, ARG46, ARG47, ARG48, ARG49, ARG50,\
                          ARG51, ARG52, ARG53, ARG54, ARG55, ARG56, ARG57, ARG58, ARG59, ARG60,\
                          ARG61, ARG62, ARG63, ARG64, ARG65, ARG66, ARG67, ARG68, ARG69, ARG70,\
                          ARG71, ARG72, ARG73, ARG74, ARG75, ARG76, ARG77, ARG78, ARG79, ARG80,\
                          ARG81, ARG82, ARG83, ARG84, ARG85, ARG86, ARG87, ARG88, ARG89, ARG90,\
                          ARG91, ARG92, ARG93, ARG94, ARG95, ARG96, ARG97, ARG98, ARG99, ARG100,\
                          ARG101, ARG102, ARG103, ARG104, ARG105, ARG106, ARG107, ARG108, ARG109, ARG110,\
                          ARG111, ARG112, ARG113, ARG114, ARG115, ARG116, ARG117, ARG118, ARG119, ARG120,\
                          ARG121, ARG122, ARG123, ARG124, ARG125, ARG126, ARG127, ARG128, ARG129, ARG130,\
                          ARG131, ARG132, ARG133, ARG134, ARG135, ARG136, ARG137, ARG138, ARG139, ARG140,\
                          ARG141, ARG142, ARG143, ARG144, ARG145, ARG146, ARG147, ARG148, ARG149, ARG150,\
                          ARG151, ARG152, ARG153, ARG154, ARG155, ARG156, ARG157, ARG158, ARG159, ARG160,\
                          ARG161, ARG162, ARG163, ARG164, ARG165, ARG166, ARG167, ARG168, ARG169, ARG170,\
                          ARG171, ARG172, ARG173, ARG174, ARG175, ARG176, ARG177, ARG178, ARG179, ARG180,\
                          ARG181, ARG182, ARG183, ARG184, ARG185, ARG186, ARG187, ARG188, ARG189, ARG190,\
                          ARG191, ARG192, ARG193, ARG194, ARG195, ARG196, ARG197, ARG198, ARG199, ARG200,\
                          ARG201, ARG202, ARG203, ARG204, ARG205, ARG206, ARG207, ARG208, ARG209, ARG210,\
                          ARG211, ARG212, ARG213, ARG214, ARG215, ARG216, ARG217, ARG218, ARG219, ARG220,\
                          ARG221, ARG222, ARG223, ARG224, ARG225, ARG226, ARG227, ARG228, ARG229, ARG230,\
                          ARG231, ARG232, ARG233, ARG234, ARG235, ARG236, ARG237, ARG238, ARG239, ARG240,\
                          ARG241, ARG242, ARG243, ARG244, ARG245, ARG246, ARG247, ARG248, ARG249, ARG250,\
                          ARG251, ARG252, ARG253, ARG254, ARG255, ARG256,  N, ...) N

#define VA_ARGS_SIZE_AUX(...) \
    VA_ARGS_SIZE_IMPL(__VA_ARGS__)
#define VA_ARGS_SIZE(...) \
    VA_ARGS_SIZE_AUX(__VA_ARGS__, INDEX_RSEQ())

#endif /* ARGSIZE_H */
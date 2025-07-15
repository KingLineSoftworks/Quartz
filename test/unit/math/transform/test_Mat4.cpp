#include "glm/fwd.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

UT_FUNCTION(test_construction) {
    // Default construction
    {
        const math::Mat4 mat;

        UT_CHECK_EQUAL(mat.col0, math::Vec4(0));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(0));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(0));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(0));

        for (const math::Vec4& col : mat.cols) {
            UT_CHECK_EQUAL(col, math::Vec4(0));
        }
    }

    // Scalar construction
    {
        const math::Mat4 mat(42);

        UT_CHECK_EQUAL(mat.col0, math::Vec4(42, 0, 0, 0));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(0, 42, 0, 0));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(0, 0, 42, 0));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(0, 0, 0, 42));
    }

    // Quaternion construction
    // This is actually very complex and I am super lazy.
    // We are wrapping glm functionality here so I think we're okay.
    // {
    //     const math::Quaternion quat(1, 2, 3, 4);
    //     const math::Mat4 mat(quat);

    //     UT_CHECK_EQUAL(mat.col0, math::Vec4(1, 0, 0, 0));
    //     UT_CHECK_EQUAL(mat.col1, math::Vec4(0, 2, 0, 0));
    //     UT_CHECK_EQUAL(mat.col2, math::Vec4(0, 0, 3, 0));
    //     UT_CHECK_EQUAL(mat.col3, math::Vec4(0, 0, 0, 4));
    // }

    // Vec4 construction
    {
        const math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(6),
                math::Vec4(7),
                math::Vec4(8),
                math::Vec4(9)
            }
        );

        UT_CHECK_EQUAL(mat.col0, math::Vec4(6, 6, 6, 6));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(7, 7, 7, 7));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(8, 8, 8, 8));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(9, 9, 9, 9));
    }

    // list construction
    {
        const math::Mat4 mat(
            {
                7, 8, 9, 0,
                0, 9, 8, 7,
                1, 2, 3, 4,
                4, 3, 2, 1
            }
        );

        UT_CHECK_EQUAL(mat.col0, math::Vec4(7, 8, 9, 0));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(0, 9, 8, 7));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(4, 3, 2, 1));
    }
}

UT_FUNCTION(test_copy) {
    // Quartz copy
    {
        const math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(1, 2, 3, 4),
                math::Vec4(5, 6, 7, 8),
                math::Vec4(9, 10, 11, 12),
                math::Vec4(13, 14, 15, 16)
            }
        );

        const math::Mat4 other(mat);

        UT_CHECK_EQUAL(mat, other);

        UT_CHECK_EQUAL(other.col0, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(other.col1, math::Vec4(5, 6, 7, 8));
        UT_CHECK_EQUAL(other.col2, math::Vec4(9, 10, 11, 12));
        UT_CHECK_EQUAL(other.col3, math::Vec4(13, 14, 15, 16));
    }
}

UT_FUNCTION(test_assignment) {
    // Quartz assignment
    {
        const math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(1, 2, 3, 4),
                math::Vec4(5, 6, 7, 8),
                math::Vec4(9, 10, 11, 12),
                math::Vec4(13, 14, 15, 16)
            }
        );

        math::Mat4 other;

        for (const math::Vec4& col : other.cols) {
            UT_CHECK_EQUAL(col, math::Vec4(0));
        }

        other = mat;

        UT_CHECK_EQUAL(other.col0, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(other.col1, math::Vec4(5, 6, 7, 8));
        UT_CHECK_EQUAL(other.col2, math::Vec4(9, 10, 11, 12));
        UT_CHECK_EQUAL(other.col3, math::Vec4(13, 14, 15, 16));
    }

    // Glm assignment
    {
        math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(1, 2, 3, 4),
                math::Vec4(5, 6, 7, 8),
                math::Vec4(9, 10, 11, 12),
                math::Vec4(13, 14, 15, 16)
            }
        );

        UT_CHECK_EQUAL(mat.col0, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(5, 6, 7, 8));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(9, 10, 11, 12));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(13, 14, 15, 16));

        const glm::mat4 glmMat(
            {
                999, 999, 999, 999,
                888, 888, 888, 888,
                777, 777, 777, 777,
                666, 666, 666, 666
            }
        );

        mat = glmMat;

        UT_CHECK_EQUAL(mat.col0, math::Vec4(999));
        UT_CHECK_EQUAL(mat.col1, math::Vec4(888));
        UT_CHECK_EQUAL(mat.col2, math::Vec4(777));
        UT_CHECK_EQUAL(mat.col3, math::Vec4(666));
    }
}

UT_FUNCTION(test_access_operators) {
    // const
    {
        const math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(1, 2, 3, 4),
                math::Vec4(5, 6, 7, 8),
                math::Vec4(9, 10, 11, 12),
                math::Vec4(13, 14, 15, 16)
            }
        );

        const math::Vec4 col0 = mat[0];
        const math::Vec4 col1 = mat[1];
        const math::Vec4 col2 = mat[2];
        const math::Vec4 col3 = mat[3];
        UT_CHECK_EQUAL(col0, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(col1, math::Vec4(5, 6, 7, 8));
        UT_CHECK_EQUAL(col2, math::Vec4(9, 10, 11, 12));
        UT_CHECK_EQUAL(col3, math::Vec4(13, 14, 15, 16));

        UT_CHECK_EQUAL(mat[0][0], 1);
        UT_CHECK_EQUAL(mat[0][1], 2);
        UT_CHECK_EQUAL(mat[0][2], 3);
        UT_CHECK_EQUAL(mat[0][3], 4);

        UT_CHECK_EQUAL(mat[1][0], 5);
        UT_CHECK_EQUAL(mat[1][1], 6);
        UT_CHECK_EQUAL(mat[1][2], 7);
        UT_CHECK_EQUAL(mat[1][3], 8);

        UT_CHECK_EQUAL(mat[2][0], 9);
        UT_CHECK_EQUAL(mat[2][1], 10);
        UT_CHECK_EQUAL(mat[2][2], 11);
        UT_CHECK_EQUAL(mat[2][3], 12);
        
        UT_CHECK_EQUAL(mat[3][0], 13);
        UT_CHECK_EQUAL(mat[3][1], 14);
        UT_CHECK_EQUAL(mat[3][2], 15);
        UT_CHECK_EQUAL(mat[3][3], 16);
    }

    // non const and assignment
    {
        math::Mat4 mat(
            std::array<math::Vec4, 4>{
                math::Vec4(1, 2, 3, 4),
                math::Vec4(5, 6, 7, 8),
                math::Vec4(9, 10, 11, 12),
                math::Vec4(13, 14, 15, 16)
            }
        );

        math::Vec4& col0 = mat[0];
        math::Vec4& col1 = mat[1];
        math::Vec4& col2 = mat[2];
        math::Vec4& col3 = mat[3];
        UT_CHECK_EQUAL(col0, math::Vec4(1, 2, 3, 4));
        UT_CHECK_EQUAL(col1, math::Vec4(5, 6, 7, 8));
        UT_CHECK_EQUAL(col2, math::Vec4(9, 10, 11, 12));
        UT_CHECK_EQUAL(col3, math::Vec4(13, 14, 15, 16));

        UT_CHECK_EQUAL(mat[0][0], 1);
        UT_CHECK_EQUAL(mat[0][1], 2);
        UT_CHECK_EQUAL(mat[0][2], 3);
        UT_CHECK_EQUAL(mat[0][3], 4);

        UT_CHECK_EQUAL(mat[1][0], 5);
        UT_CHECK_EQUAL(mat[1][1], 6);
        UT_CHECK_EQUAL(mat[1][2], 7);
        UT_CHECK_EQUAL(mat[1][3], 8);

        UT_CHECK_EQUAL(mat[2][0], 9);
        UT_CHECK_EQUAL(mat[2][1], 10);
        UT_CHECK_EQUAL(mat[2][2], 11);
        UT_CHECK_EQUAL(mat[2][3], 12);
        
        UT_CHECK_EQUAL(mat[3][0], 13);
        UT_CHECK_EQUAL(mat[3][1], 14);
        UT_CHECK_EQUAL(mat[3][2], 15);
        UT_CHECK_EQUAL(mat[3][3], 16);

        // now assign to the matrix directly and ensure we
        // have the correct values
        
        mat[0][0] = -1;
        mat[0][1] = -2;
        mat[0][2] = -3;
        mat[0][3] = -4;

        mat[1][0] = -5;
        mat[1][1] = -6;
        mat[1][2] = -7;
        mat[1][3] = -8;

        mat[2][0] = -9;
        mat[2][1] = -10;
        mat[2][2] = -11;
        mat[2][3] = -12;
        
        mat[3][0] = -13;
        mat[3][1] = -14;
        mat[3][2] = -15;
        mat[3][3] = -16;
        
        UT_CHECK_EQUAL(mat[0][0], -1);
        UT_CHECK_EQUAL(mat[0][1], -2);
        UT_CHECK_EQUAL(mat[0][2], -3);
        UT_CHECK_EQUAL(mat[0][3], -4);

        UT_CHECK_EQUAL(mat[1][0], -5);
        UT_CHECK_EQUAL(mat[1][1], -6);
        UT_CHECK_EQUAL(mat[1][2], -7);
        UT_CHECK_EQUAL(mat[1][3], -8);

        UT_CHECK_EQUAL(mat[2][0], -9);
        UT_CHECK_EQUAL(mat[2][1], -10);
        UT_CHECK_EQUAL(mat[2][2], -11);
        UT_CHECK_EQUAL(mat[2][3], -12);
        
        UT_CHECK_EQUAL(mat[3][0], -13);
        UT_CHECK_EQUAL(mat[3][1], -14);
        UT_CHECK_EQUAL(mat[3][2], -15);
        UT_CHECK_EQUAL(mat[3][3], -16);

        // now assign to the column references and ensure we
        // get updates in the actual matrix

        mat[0] = math::Vec4(111.111);
        mat[1] = math::Vec4(-222.222);
        mat[2] = math::Vec4(-333.333);
        mat[3] = math::Vec4(-444.444);
        
        UT_CHECK_EQUAL_FLOATS(mat[0][0], 111.111);
        UT_CHECK_EQUAL_FLOATS(mat[0][1], 111.111);
        UT_CHECK_EQUAL_FLOATS(mat[0][2], 111.111);
        UT_CHECK_EQUAL_FLOATS(mat[0][3], 111.111);

        UT_CHECK_EQUAL_FLOATS(mat[1][0], -222.222);
        UT_CHECK_EQUAL_FLOATS(mat[1][1], -222.222);
        UT_CHECK_EQUAL_FLOATS(mat[1][2], -222.222);
        UT_CHECK_EQUAL_FLOATS(mat[1][3], -222.222);

        UT_CHECK_EQUAL_FLOATS(mat[2][0], -333.333);
        UT_CHECK_EQUAL_FLOATS(mat[2][1], -333.333);
        UT_CHECK_EQUAL_FLOATS(mat[2][2], -333.333);
        UT_CHECK_EQUAL_FLOATS(mat[2][3], -333.333);
        
        UT_CHECK_EQUAL_FLOATS(mat[3][0], -444.444);
        UT_CHECK_EQUAL_FLOATS(mat[3][1], -444.444);
        UT_CHECK_EQUAL_FLOATS(mat[3][2], -444.444);
        UT_CHECK_EQUAL_FLOATS(mat[3][3], -444.444);

        UT_CHECK_EQUAL(col0, mat[0]);
        UT_CHECK_EQUAL(col1, mat[1]);
        UT_CHECK_EQUAL(col2, mat[2]);
        UT_CHECK_EQUAL(col3, mat[3]);
    }
}

UT_FUNCTION(test_matrix_operators) {
    // multiply with a quartz matrix
    {
        const math::Mat4 a(
            {
                1, 2, 3, 4,
                4, 3, 2, 1,
                1, 1, 2, 2,
                3, 3, 4, 4
            }
        );

        math::Mat4 aNC = a;

        const math::Mat4 b(
            {
                6, 7, 8, 9,
                9, 8, 7, 6,
                6, 6, 7, 7,
                8, 8, 9, 9
            }
        );

        const math::Mat4 c = a * b;

        UT_CHECK_EQUAL(c[0][0], 1*6 + 4*7 + 1*8 + 3*9);
        UT_CHECK_EQUAL(c[0][1], 2*6 + 3*7 + 1*8 + 3*9);
        UT_CHECK_EQUAL(c[0][2], 3*6 + 2*7 + 2*8 + 4*9);
        UT_CHECK_EQUAL(c[0][3], 4*6 + 1*7 + 2*8 + 4*9);

        UT_CHECK_EQUAL(c[1][0], 1*9 + 4*8 + 1*7 + 3*6);
        UT_CHECK_EQUAL(c[1][1], 2*9 + 3*8 + 1*7 + 3*6);
        UT_CHECK_EQUAL(c[1][2], 3*9 + 2*8 + 2*7 + 4*6);
        UT_CHECK_EQUAL(c[1][3], 4*9 + 1*8 + 2*7 + 4*6);

        UT_CHECK_EQUAL(c[2][0], 1*6 + 4*6 + 1*7 + 3*7);
        UT_CHECK_EQUAL(c[2][1], 2*6 + 3*6 + 1*7 + 3*7);
        UT_CHECK_EQUAL(c[2][2], 3*6 + 2*6 + 2*7 + 4*7);
        UT_CHECK_EQUAL(c[2][3], 4*6 + 1*6 + 2*7 + 4*7);

        UT_CHECK_EQUAL(c[3][0], 1*8 + 4*8 + 1*9 + 3*9);
        UT_CHECK_EQUAL(c[3][1], 2*8 + 3*8 + 1*9 + 3*9);
        UT_CHECK_EQUAL(c[3][2], 3*8 + 2*8 + 2*9 + 4*9);
        UT_CHECK_EQUAL(c[3][3], 4*8 + 1*8 + 2*9 + 4*9);

        aNC *= b;

        UT_CHECK_EQUAL(aNC, c);
    }

    // multiply with a glm matrix
    {
        const math::Mat4 a(
            {
                1, 2, 3, 4,
                4, 3, 2, 1,
                1, 1, 2, 2,
                3, 3, 4, 4
            }
        );

        math::Mat4 aNC = a;

        const glm::mat4 b(
            {
                6, 7, 8, 9,
                9, 8, 7, 6,
                6, 6, 7, 7,
                8, 8, 9, 9
            }
        );

        const math::Mat4 c = a * b;

        UT_CHECK_EQUAL(c[0][0], 1*6 + 4*7 + 1*8 + 3*9);
        UT_CHECK_EQUAL(c[0][1], 2*6 + 3*7 + 1*8 + 3*9);
        UT_CHECK_EQUAL(c[0][2], 3*6 + 2*7 + 2*8 + 4*9);
        UT_CHECK_EQUAL(c[0][3], 4*6 + 1*7 + 2*8 + 4*9);

        UT_CHECK_EQUAL(c[1][0], 1*9 + 4*8 + 1*7 + 3*6);
        UT_CHECK_EQUAL(c[1][1], 2*9 + 3*8 + 1*7 + 3*6);
        UT_CHECK_EQUAL(c[1][2], 3*9 + 2*8 + 2*7 + 4*6);
        UT_CHECK_EQUAL(c[1][3], 4*9 + 1*8 + 2*7 + 4*6);

        UT_CHECK_EQUAL(c[2][0], 1*6 + 4*6 + 1*7 + 3*7);
        UT_CHECK_EQUAL(c[2][1], 2*6 + 3*6 + 1*7 + 3*7);
        UT_CHECK_EQUAL(c[2][2], 3*6 + 2*6 + 2*7 + 4*7);
        UT_CHECK_EQUAL(c[2][3], 4*6 + 1*6 + 2*7 + 4*7);

        UT_CHECK_EQUAL(c[3][0], 1*8 + 4*8 + 1*9 + 3*9);
        UT_CHECK_EQUAL(c[3][1], 2*8 + 3*8 + 1*9 + 3*9);
        UT_CHECK_EQUAL(c[3][2], 3*8 + 2*8 + 2*9 + 4*9);
        UT_CHECK_EQUAL(c[3][3], 4*8 + 1*8 + 2*9 + 4*9);

        aNC *= b;

        UT_CHECK_EQUAL(aNC, c);
    }

    // equals
    {
        const math::Mat4 a(
            std::array<math::Vec4, 4>{
                math::Vec4(99),
                math::Vec4(88),
                math::Vec4(77),
                math::Vec4(66)
            }
        );

        const math::Mat4 b(
            std::array<math::Vec4, 4>{
                math::Vec4(99),
                math::Vec4(88),
                math::Vec4(77),
                math::Vec4(66)
            }
        );

        UT_CHECK_TRUE(a == b);
        UT_CHECK_FALSE(a != b);
    }

    // not equals
    {
        const math::Mat4 a(
            std::array<math::Vec4, 4>{
                math::Vec4(99),
                math::Vec4(88),
                math::Vec4(77),
                math::Vec4(66)
            }
        );

        const math::Mat4 b(
            std::array<math::Vec4, 4>{
                math::Vec4(99, 99, 99, 98),
                math::Vec4(88),
                math::Vec4(77),
                math::Vec4(66)
            }
        );

        UT_CHECK_TRUE(a != b);
        UT_CHECK_FALSE(a == b);
    }
}

UT_FUNCTION(test_translate) {
    const math::Mat4 a(
        {
            1, 2, 3, 4,
            -1, -2, -3, -4,
            6, 7, 8, 9,
            -6, -7, -8, -9
        }
    );

    const math::Vec3 translation(42, 69, -7);

    const math::Mat4 translationMat(
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translation.x, translation.y, translation.z, 1
        }
    );

    const math::Mat4 expected = a * translationMat;
    const math::Mat4 c = math::Mat4::translate(a, translation);

    UT_CHECK_EQUAL(c, expected);

    math::Mat4 aNC = a;
    
    aNC.translate(translation);

    UT_CHECK_EQUAL(aNC, expected);
}

UT_FUNCTION(test_rotate) {
    // Though I may claim that I am a decent software engineer, I will not deny that
    // I can be lazy and do not always want to write tests for complex functions
    // implemented in ubiquitous and industry standard libraries of which I am merely
    // wrapping
    //
    // GLM functions are trustworthy IMO. If I ever for some reason decide to do away
    // with GLM in Quartz then I will write tests for this
}

UT_FUNCTION(test_scale) {
    const math::Mat4 a(
        {
            1, 2, 3, 4,
            -1, -2, -3, -4,
            6, 7, 8, 9,
            -6, -7, -8, -9
        }
    );

    const math::Vec3 scale(10, 2, 0.5);

    const math::Mat4 scaleMat(
        {
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
        }
    );

    const math::Mat4 expected = a * scaleMat;
    const math::Mat4 c = math::Mat4::scale(a, scale);

    UT_CHECK_EQUAL(c, expected);

    math::Mat4 aNC = a;

    aNC.scale(scale);

    UT_CHECK_EQUAL(aNC, expected);
}

UT_FUNCTION(test_createPerspective) {
    // Go read the comment under test_rotate for why there is no test there.
    // Now apply that comment to this function as well. I trust that GLM is
    // doing correct maths. If they aren't, then I guess I will find out some
    // how in some catastrophic manner ...
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_copy);
    REGISTER_UT_FUNCTION(test_assignment);
    REGISTER_UT_FUNCTION(test_access_operators);
    REGISTER_UT_FUNCTION(test_matrix_operators);
    REGISTER_UT_FUNCTION(test_translate);
    REGISTER_UT_FUNCTION(test_rotate);
    REGISTER_UT_FUNCTION(test_scale);
    REGISTER_UT_FUNCTION(test_createPerspective);
    UT_RUN_TESTS();
}


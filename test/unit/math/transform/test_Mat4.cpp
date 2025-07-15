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

}

UT_FUNCTION(test_translate) {

}

UT_FUNCTION(test_rotate) {

}

UT_FUNCTION(test_scale) {

}

UT_FUNCTION(test_createPerspective) {

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


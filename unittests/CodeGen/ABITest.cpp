#include "CodeGen/ABI.hpp"
#include "gtest/gtest.h"

class ABITest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        context_ = std::make_unique<llvm::LLVMContext>();
        module_ = std::make_unique<llvm::Module>("Module", *context_);
        abi_ = std::make_unique<CodeGen::X86_64ABI>(*module_);
    }

    void TearDown() override
    {
    }

    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<CodeGen::ABI> abi_;
};

TEST_F(ABITest, getFunctionType_Basic)
{
    auto retType = llvm::Type::getInt32Ty(*context_);
    auto paramTypes =
        std::vector<llvm::Type *>{llvm::Type::getInt32Ty(*context_)};
    auto type = abi_->getFunctionType(retType, paramTypes);

    EXPECT_EQ(type->getNumParams(), 1);
    EXPECT_EQ(type->getReturnType(), llvm::Type::getInt32Ty(*context_));
}

TEST_F(ABITest, getFunctionType_only6IntRegs)
{
    auto structType1Reg = llvm::StructType::create(*context_, "pair");
    structType1Reg->setBody(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 1));
    auto structType2Reg = llvm::StructType::create(*context_, "pair");
    structType2Reg->setBody(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 9));

    // Test 1
    auto retType = llvm::Type::getInt32Ty(*context_);
    auto paramTypes = std::vector<llvm::Type *>{
        structType1Reg,
        structType1Reg,
        structType1Reg,
        structType1Reg,
        structType1Reg,
        structType1Reg,
        structType2Reg,
        structType2Reg};
    auto expectedParamTypes = std::vector<llvm::Type *>{
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::PointerType::get(*context_, 0),
        llvm::PointerType::get(*context_, 0)};
    auto type = abi_->getFunctionType(retType, paramTypes);

    EXPECT_EQ(type->getNumParams(), expectedParamTypes.size());
    EXPECT_EQ(type->getReturnType(), llvm::Type::getInt32Ty(*context_));
    for (int i = 0; i < expectedParamTypes.size(); i++)
    {
        EXPECT_EQ(type->getParamType(i), expectedParamTypes[i]);
    }

    // Test 2
    paramTypes = std::vector<llvm::Type *>{
        structType2Reg,
        structType2Reg,
        structType2Reg,
        structType2Reg,
        structType1Reg};
    expectedParamTypes = std::vector<llvm::Type *>{
        llvm::Type::getInt64Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt64Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::Type::getInt64Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
        llvm::PointerType::get(*context_, 0),
        llvm::Type::getInt8Ty(*context_)};
    type = abi_->getFunctionType(retType, paramTypes);

    EXPECT_EQ(type->getNumParams(), expectedParamTypes.size());
    EXPECT_EQ(type->getReturnType(), llvm::Type::getInt32Ty(*context_));
    for (int i = 0; i < expectedParamTypes.size(); i++)
    {
        EXPECT_EQ(type->getParamType(i), expectedParamTypes[i]);
    }

    // Test 3 - 5 regs used, cannot expand into 2 (5 + 2 > 6)
    paramTypes = std::vector<llvm::Type *>{
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        structType2Reg};
    expectedParamTypes = std::vector<llvm::Type *>{
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt32Ty(*context_),
        llvm::PointerType::get(*context_, 0)};
    type = abi_->getFunctionType(retType, paramTypes);

    EXPECT_EQ(type->getNumParams(), expectedParamTypes.size());
    EXPECT_EQ(type->getReturnType(), llvm::Type::getInt32Ty(*context_));
    for (int i = 0; i < expectedParamTypes.size(); i++)
    {
        EXPECT_EQ(type->getParamType(i), expectedParamTypes[i]);
    }
}

TEST_F(ABITest, getFunctionType_retval)
{
    auto structType2Reg = llvm::StructType::create(*context_, "pair");
    structType2Reg->setBody(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 9));

    auto retType = structType2Reg;
    auto paramTypes = std::vector<llvm::Type *>{};
    auto type = abi_->getFunctionType(retType, paramTypes);

    // struct.pair -> { i64, i8 }
    EXPECT_EQ(type->getNumParams(), 0);
    EXPECT_EQ(
        type->getReturnType(),
        llvm::StructType::get(
            *context_,
            {llvm::Type::getInt64Ty(*context_),
             llvm::Type::getInt8Ty(*context_)}));
}

TEST_F(ABITest, getParamType_Basic)
{
    auto type = abi_->getParamType(llvm::Type::getInt32Ty(*context_));

    EXPECT_EQ(type.size(), 1);
    EXPECT_EQ(type[0], llvm::Type::getInt32Ty(*context_));
}

TEST_F(ABITest, getParamType_BasicStruct)
{
    auto structType = llvm::StructType::create(*context_, "pair");
    structType->setBody(
        {llvm::Type::getInt32Ty(*context_),
         llvm::Type::getDoubleTy(*context_)});
    auto type = abi_->getParamType(structType);

    EXPECT_EQ(type.size(), 2);
    EXPECT_EQ(type[0], llvm::Type::getInt32Ty(*context_));
    EXPECT_EQ(type[1], llvm::Type::getDoubleTy(*context_));
}

TEST_F(ABITest, getParamType_AlignStruct)
{
    auto structType = llvm::StructType::create(*context_, "pair");
    structType->setBody({
        llvm::Type::getInt32Ty(*context_),
        llvm::Type::getInt8Ty(*context_),
    });
    auto type = abi_->getParamType(structType);

    EXPECT_EQ(type.size(), 1);
    EXPECT_EQ(type[0], llvm::Type::getInt64Ty(*context_));
}

TEST_F(ABITest, getParamType_ArrayStruct)
{
    auto structType = llvm::StructType::create(*context_, "pair");
    structType->setBody(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 11));
    auto type = abi_->getParamType(structType);

    EXPECT_EQ(type.size(), 2);
    EXPECT_EQ(type[0], llvm::Type::getInt64Ty(*context_));
    EXPECT_EQ(type[1], llvm::Type::getIntNTy(*context_, 24));
}

TEST_F(ABITest, getTypeAlign)
{
    auto align = abi_->getTypeAlign(llvm::Type::getInt32Ty(*context_));
    EXPECT_EQ(align, llvm::Align(4));
}

TEST_F(ABITest, getTypeSize)
{
    auto size = abi_->getTypeSize(AST::Types::INT);
    EXPECT_EQ(size, 32);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
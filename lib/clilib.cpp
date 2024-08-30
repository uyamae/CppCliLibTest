namespace clilib {
    public ref class TestClass {
    public:
        static void PrintMessage(System::String ^ message);
    };
    
    void TestClass::PrintMessage(System::String ^ message)
    {
        System::Console::WriteLine(message);
    }
}
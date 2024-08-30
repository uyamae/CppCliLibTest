#using "clilib.dll"

int main()
{
    clilib::TestClass::PrintMessage(gcnew System::String("hello!"));
    return 0;
}
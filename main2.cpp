
int a = 5;
int b = 5;
int c = []()
{
    return a + b;
}();
main()
{
    a = a + b;
    return 0;
}
int global_vec[20];

float process_data(float data1, float data2)
{
    float result <= 0;

    result = data1 + data2;
    
    result = result*global_vec[1];

    return result;
}

string main(int argc, char argv)
{
    float var <= 0;
    int user_input <= 0;
    string result <= "O resultado e ";

    // Get user input
    input user_input;

    var = process_data(user_input, global_vec[2 + 2]);

    // Check if less than 10
    if (var >= 10)
    {
        output true;

        result = result + " maior q 10";
    }
    else
    {
        output false;

        result = result + " menor q 10";
    };
    

    return result;
}
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

const string CLIENTS_FILE_NAME = "clients-list.txt";
const string Users_FILE_NAME = "users-list.txt";

struct stClient
{
    string AccountNumber, PINCode, Name, Phone;
    double AccountBalance;
    bool MarkForDelete = false;
};

struct stUser
{
    string UserName, Password;
    int Permissions = 0;
    bool MarkForDelete = false;
};

enum enMainMenuOptions
{
    eListClients = 1,
    eAddClients = 2,
    eDeleteClient = 3,
    eUpdateClient = 4,
    eFindClient = 5,
    eTransactions = 6,
    eManageUsers = 7,
    eLogOut = 8
};

enum enTransactionsMenuOptions
{
    eDeposit = 1,
    eWithdraw = 2,
    eTotalBalance = 3,
    eMainMenu = 4
};

enum enManageUsersMenuOptions
{
    eListUsers = 1,
    eAddUsers = 2,
    eDeleteUser = 3,
    eUpdateUser = 4,
    eFindUser = 5,
    eMainMenuScreen = 6
};

enum enMainMenuPermissions
{
    pAll = -1,
    pListClients = 1,
    pAddClients = 2,
    pDeleteClient = 4,
    pUpdateClient = 8,
    pFindClient = 16,
    pTransactions = 32,
    pManageUsers = 64
};

stUser CurrentUser;

void ShowMainMenuScreen();

void ShowTransactionsMenu();

void ShowManageUsersMenu();

void GoBackToMainMenu();

void Login();

vector<string> SplitString(string Str, string Delim)
{
    int pos = 0;
    string sWord = "";
    vector<string> vString;

    while ((pos = Str.find(Delim)) != string::npos)
    {
        sWord = Str.substr(0, pos);

        if (sWord != "")
            vString.push_back(sWord);

        Str.erase(0, pos + Delim.length());
    }

    if (Str != "")
        vString.push_back(Str);

    return vString;
}

stClient ConvertDataLineToRecord(string DataLine, string Separator = "#//#")
{
    vector<string> vString = SplitString(DataLine, Separator);
    stClient Client;

    Client.AccountNumber = vString[0];
    Client.PINCode = vString[1];
    Client.Name = vString[2];
    Client.Phone = vString[3];
    Client.AccountBalance = stod(vString[4]);

    return Client;
}

vector<stClient> LoadClientDataFromFile(string FileName)
{
    fstream MyFile;
    vector<stClient> vClients;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line = "";
        stClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertDataLineToRecord(Line);
            vClients.push_back(Client);
        }

        MyFile.close();
    }

    return vClients;
}

void PrintClientRecord(stClient Client)
{
    cout << "| " << left << setw(15) << Client.AccountNumber;
    cout << "| " << left << setw(10) << Client.PINCode;
    cout << "| " << left << setw(35) << Client.Name;
    cout << "| " << left << setw(15) << Client.Phone;
    cout << "| " << left << setw(12) << Client.AccountBalance;
}

void ShowAllClients(vector<stClient> vClients)
{
    cout << "\n\t\t\t\tClient List (" << vClients.size() << ") client(s)." << endl;
    cout << "________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(10) << "PIN Code";
    cout << "| " << left << setw(35) << "Client Name";
    cout << "| " << left << setw(15) << "Phone";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n________________________________________________________________________________________________\n\n";

    if (vClients.size() == 0)
        cout << "\t\t\t\t No Client available in the system!";
    else
    {
        for (stClient &Client : vClients)
        {
            PrintClientRecord(Client);
            cout << endl;
        }
    }
    cout << "\n\n_________________________________________________________________________________________________\n";
}

bool CheckAccessPermission(enMainMenuPermissions Permission)
{
    if (CurrentUser.Permissions == enMainMenuPermissions::pAll)
        return true;
    else if ((Permission & CurrentUser.Permissions) == Permission)
        return true;
    else
        return false;
}

void ShowAccessDeniedMessage()
{
    cout << "\n------------------------------\n";
    cout << "Access Denied,\nYou Do Not Have Permission To Do This,\nPlease Contact Your Admin.\n";
    cout << "------------------------------\n";
}

void ShowAllClientsScreen()
{

    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pListClients))
    {
        ShowAccessDeniedMessage();
        return;
    }

    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);

    ShowAllClients(vClients);
}

bool IsAccountNumberExist(string AccountNumber, string FileName)
{
    fstream MyFile;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        stClient Client;
        string Line;

        while (getline(MyFile, Line))
        {
            Client = ConvertDataLineToRecord(Line);

            if (Client.AccountNumber == AccountNumber)
            {
                MyFile.close();

                return true;
            }
        }
        MyFile.close();
    }

    return false;
}

stClient ReadClientRecord()
{
    stClient Client;

    cout << "Please Enter Account Number: ";
    getline(cin >> ws, Client.AccountNumber);

    while (IsAccountNumberExist(Client.AccountNumber, CLIENTS_FILE_NAME))
    {
        cout << "Account Number (" << Client.AccountNumber << ") Exist. Please enter another account number: ";
        getline(cin, Client.AccountNumber);
    }

    cout << "Please Enter PIN Code: ";
    getline(cin, Client.PINCode);
    cout << "Please Enter Name: ";
    getline(cin, Client.Name);
    cout << "Please Enter Phone: ";
    getline(cin, Client.Phone);
    cout << "Please Enter Account Balance: ";
    cin >> Client.AccountBalance;

    return Client;
}

string ConvertRecordToDataLine(stClient Client, string Separator = "#//#")
{
    string DataLine = "";

    DataLine += Client.AccountNumber + Separator;
    DataLine += Client.PINCode + Separator;
    DataLine += Client.Name + Separator;
    DataLine += Client.Phone + Separator;
    DataLine += to_string(Client.AccountBalance);

    return DataLine;
}

void AddDataLineToFile(string FileName, string DataLine)
{
    fstream MyFile;

    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    {
        MyFile << DataLine << endl;
        MyFile.close();
    }
}

void AddNewClient()
{
    stClient Client = ReadClientRecord();

    AddDataLineToFile(CLIENTS_FILE_NAME, ConvertRecordToDataLine(Client));
}

void AddNewClients()
{
    char AddMore = 'y';

    do
    {

        cout << "\nAdding New Client:\n\n";

        AddNewClient();

        cout << "\nClient Added Successfully, do you want to add more clients: ";
        cin >> AddMore;

    } while (tolower(AddMore) == 'y');
}

void ShowAddClientsScreen()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pAddClients))
    {
        ShowAccessDeniedMessage();
        return;
    }

    cout << "==================================\n";
    cout << "\tAdd New Clients Screen\n";
    cout << "==================================\n";

    AddNewClients();
}

string ReadAccountNumber()
{
    string AccountNumber;

    cout << "\nPlease enter Account Number: ";
    getline(cin >> ws, AccountNumber);

    return AccountNumber;
}

void PrintClientCard(stClient Client)
{
    cout << "\nThe followings are Client Details:\n";
    cout << "----------------------------------------\n";
    cout << "Account Number: " << Client.AccountNumber << endl;
    cout << "PIN Code: " << Client.PINCode << endl;
    cout << "Name: " << Client.Name << endl;
    cout << "Phone: " << Client.Phone << endl;
    cout << "Account Balance: " << Client.AccountBalance << endl;
    cout << "----------------------------------------\n";
}

void MarkClientForDelete(vector<stClient> &vClients, string AccountNumber)
{
    for (stClient &C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            break;
        }
    }
}

void SaveClientsToFile(vector<stClient> vClients, string FileName)
{
    fstream MyFile;

    MyFile.open(FileName, ios::out);

    if (MyFile.is_open())
    {
        for (stClient &C : vClients)
        {
            if (!C.MarkForDelete)
                MyFile << ConvertRecordToDataLine(C) << endl;
        }
    }
}

bool FindClientByAccountNumber(string AccountNumber, stClient &Client, vector<stClient> vClients)
{
    for (stClient &C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }
    }

    return false;
}

void DeleteClientByAccountNumber(vector<stClient> &vClients, string AccountNumber)
{
    stClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, Client, vClients))
    {
        PrintClientCard(Client);

        cout << "\nAre you sure you want to delete this client? y/n? ";
        cin >> Answer;

        if (tolower(Answer) == 'y')
        {
            MarkClientForDelete(vClients, AccountNumber);
            SaveClientsToFile(vClients, CLIENTS_FILE_NAME);
            vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);

            cout << "\nClient Deleted Successfully!\n";
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") Not Found!\n";
    }
}

void ShowDeleteClientScreen()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pDeleteClient))
    {
        ShowAccessDeniedMessage();
        return;
    }

    cout << "==================================\n";
    cout << "\tDelete Client Screen\n";
    cout << "==================================\n";

    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);
    string AccountNumber = ReadAccountNumber();
    DeleteClientByAccountNumber(vClients, AccountNumber);
}

stClient UpdateClientRecord(string AccountNumber)
{
    stClient Client;

    Client.AccountNumber = AccountNumber;

    cout << "\nPlease Enter PIN Code: ";
    getline(cin >> ws, Client.PINCode);

    cout << "Please Enter Name: ";
    getline(cin, Client.Name);

    cout << "Please Enter Phone: ";
    getline(cin, Client.Phone);

    cout << "Please Enter Account Balance: ";
    cin >> Client.AccountBalance;

    return Client;
}

void UpdateClientByAccountNumber(vector<stClient> &vClients, string AccountNumber)
{
    stClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, Client, vClients))
    {
        PrintClientCard(Client);

        cout << "\nAre you sure you want to update this client? y/n? ";
        cin >> Answer;

        if (tolower(Answer) == 'y')
        {
            for (stClient &C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    C = UpdateClientRecord(AccountNumber);
                    break;
                }
            }

            SaveClientsToFile(vClients, CLIENTS_FILE_NAME);

            cout << "\nClient Updated Successfully\n";
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") Not Found!\n";
    }
}

void ShowUpdateClientScreen()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pUpdateClient))
    {
        ShowAccessDeniedMessage();
        return;
    }

    cout << "==================================\n";
    cout << "\tUpdate Client Screen\n";
    cout << "==================================\n";

    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);
    string AccountNumber = ReadAccountNumber();
    UpdateClientByAccountNumber(vClients, AccountNumber);
}

void ShowFindClientScreen()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pFindClient))
    {
        ShowAccessDeniedMessage();
        return;
    }

    cout << "==================================\n";
    cout << "\tFind Client Screen\n";
    cout << "==================================\n";

    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);
    string AccountNumber = ReadAccountNumber();
    stClient Client;

    if (FindClientByAccountNumber(AccountNumber, Client, vClients))
        PrintClientCard(Client);
    else
        cout << "\nClient with Account Number (" << AccountNumber << ") Not Found!\n";
}

void ApplyTransaction(vector<stClient> &vClients, string AccountNumber, double Amount)
{
    char Answer = 'n';

    cout << "\nAre you sure you want to perform this transactions: ";
    cin >> Answer;

    if (tolower(Answer) == 'y')
    {
        for (stClient &Client : vClients)
        {
            if (Client.AccountNumber == AccountNumber)
            {
                Client.AccountBalance += Amount;
                SaveClientsToFile(vClients, CLIENTS_FILE_NAME);
                cout << "\nDone Successfully. New Balance now is: " << Client.AccountBalance << endl;
                break;
            }
        }
    }
}

void DepositBalanceByAccountNumber()
{
    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);
    stClient Client;
    double Amount = 0;
    string AccountNumber = ReadAccountNumber();

    while (!FindClientByAccountNumber(AccountNumber, Client, vClients))
    {
        AccountNumber = ReadAccountNumber();
    }

    PrintClientCard(Client);

    cout << "\nPlease enter deposit amount: ";
    cin >> Amount;

    ApplyTransaction(vClients, AccountNumber, Amount);
}

void ShowDepositScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tDeposit Screen\n";
    cout << "==================================\n";
    DepositBalanceByAccountNumber();
}

void WithdrawBalanceByAccountNumber()
{
    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);
    stClient Client;
    double Amount = 0;
    string AccountNumber = ReadAccountNumber();

    while (!FindClientByAccountNumber(AccountNumber, Client, vClients))
    {
        AccountNumber = ReadAccountNumber();
    }

    PrintClientCard(Client);

    cout << "\nPlease enter withdraw amount: ";
    cin >> Amount;

    while (Amount > Client.AccountBalance)
    {
        cout << "\nAmount Exceed the balance, you can withdraw up to: " << Client.AccountBalance << endl;
        cout << "Please enter another amount: ";
        cin >> Amount;
    }

    ApplyTransaction(vClients, AccountNumber, Amount * -1);
}

void ShowWithdrawScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tWithdraw Screen\n";
    cout << "==================================\n";
    WithdrawBalanceByAccountNumber();
}

void PrintClientInfo(stClient Client)
{
    cout << "| " << left << setw(15) << Client.AccountNumber;
    cout << "| " << left << setw(40) << Client.Name;
    cout << "| " << left << setw(20) << Client.AccountBalance;
}

void ShowBalanceList(vector<stClient> vClients)
{

    cout << "\n\t\t\t\tBalance List (" << vClients.size() << ") client(s)." << endl;
    cout << "________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(20) << "Balance";
    cout << "\n________________________________________________________________________________________________\n\n";

    double TotalBalance = 0;

    if (vClients.size() == 0)
        cout << "\t\t\t\t No Client available in the system!";
    else
    {
        for (stClient &Client : vClients)
        {
            PrintClientInfo(Client);
            TotalBalance += Client.AccountBalance;
            cout << endl;
        }
    }
    cout << "\n\n_________________________________________________________________________________________________\n";

    cout << "\n\t\t\t\t\t Total Balances = " << std::fixed << std::setprecision(2) << TotalBalance << endl;
}

void ShowTotalBalanceScreen()
{
    system("cls");
    vector<stClient> vClients = LoadClientDataFromFile(CLIENTS_FILE_NAME);

    ShowBalanceList(vClients);
}

short ReadTransactionsMenuOpetion()
{
    short Option;

    do
    {
        cout << "Choose what you want to do? [1 to 4]? ";
        cin >> Option;

    } while (Option < 1 || Option > 4);

    return Option;
}

void GoBackToTransactionsMenu()
{
    cout << "\n\nPlease press any key to go back to transactions menu...";
    system("pause>0");
    ShowTransactionsMenu();
}

void PerformTransactionsMenuOption(enTransactionsMenuOptions TransactionsMenuOption)
{
    switch (TransactionsMenuOption)
    {
    case enTransactionsMenuOptions::eDeposit:
        ShowDepositScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenuOptions::eWithdraw:
        ShowWithdrawScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenuOptions::eTotalBalance:
        ShowTotalBalanceScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenuOptions::eMainMenu:
        ShowMainMenuScreen();
        break;
    }
}

void ShowTransactionsMenu()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pTransactions))
    {
        ShowAccessDeniedMessage();
        GoBackToMainMenu();
        return;
    }

    cout << "============================================\n";
    cout << "\tTransactions Menu Screen\n";
    cout << "============================================\n";
    cout << "\t[1] Deposit.\n";
    cout << "\t[2] Withdraw.\n";
    cout << "\t[3] Total Balances.\n";
    cout << "\t[4] Main Menu.\n";
    cout << "============================================\n";

    PerformTransactionsMenuOption((enTransactionsMenuOptions)ReadTransactionsMenuOpetion());
}

void GoBackToMainMenu()
{
    cout << "\n\nPlease press any key to go back to main menu...";
    system("pause>0");
    ShowMainMenuScreen();
}

stUser ConvertDataLineToUserRecord(string DataLine, string Separator = "#//#")
{
    vector<string> vString = SplitString(DataLine, Separator);
    stUser User;

    User.UserName = vString[0];
    User.Password = vString[1];
    User.Permissions = stoi(vString[2]);

    return User;
}

vector<stUser> LoadUsersDataFromFile(string FileName)
{
    fstream MyFile;
    vector<stUser> vUsers;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line = "";
        stUser User;

        while (getline(MyFile, Line))
        {
            User = ConvertDataLineToUserRecord(Line);
            vUsers.push_back(User);
        }

        MyFile.close();
    }

    return vUsers;
}

short ReadManageUsersMenuOpetion()
{
    short Option;

    do
    {
        cout << "Choose what you want to do? [1 to 6]? ";
        cin >> Option;

    } while (Option < 1 || Option > 6);

    return Option;
}

void PrintUserRecord(stUser User)
{
    cout << "| " << left << setw(30) << User.UserName;
    cout << "| " << left << setw(20) << User.Password;
    cout << "| " << left << setw(10) << User.Permissions;
}

void ShowAllUsers(vector<stUser> vUsers)
{
    cout << "\n\t\t\t\tUsers List (" << vUsers.size() << ") User(s)." << endl;
    cout << "________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(30) << "UserName";
    cout << "| " << left << setw(20) << "Password";
    cout << "| " << left << setw(10) << "Permissions";
    cout << "\n________________________________________________________________________________________________\n\n";

    if (vUsers.size() == 0)
        cout << "\t\t\t\t No User available in the system!";
    else
    {
        for (stUser &User : vUsers)
        {
            PrintUserRecord(User);
            cout << endl;
        }
    }
    cout << "\n\n_________________________________________________________________________________________________\n";
}

void ShowAllUsersScreen()
{
    system("cls");

    vector<stUser> vUsers = LoadUsersDataFromFile(Users_FILE_NAME);

    ShowAllUsers(vUsers);
}

void GoBackToManageUsersMenu()
{
    cout << "\n\nPlease press any key to go back to manage users menu...";
    system("pause>0");
    ShowManageUsersMenu();
}

string ConvertUserRecordToDataLine(stUser User, string Separator = "#//#")
{
    string DataLine = "";

    DataLine += User.UserName + Separator;
    DataLine += User.Password + Separator;
    DataLine += to_string(User.Permissions);

    return DataLine;
}

short ReadPermissionsToSet()
{
    int Permissions = 0;
    char Answer;

    cout << "\nDo you want to give full access? y/n? ";
    cin >> Answer;

    if (tolower(Answer) == 'y')
        Permissions = enMainMenuPermissions::pAll;

    else
    {
        cout << "\nDo you want to give access to:\n";

        cout << "\nShow Clients Lists: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pListClients;

        cout << "\nAdd New Clients: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pAddClients;

        cout << "\nDelete Client: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pDeleteClient;

        cout << "\nUpdate Client: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pUpdateClient;

        cout << "\nFind Client: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pFindClient;

        cout << "\nTransactions: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pTransactions;

        cout << "\nManage Users: y/n? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
            Permissions += enMainMenuPermissions::pManageUsers;
    }

    return Permissions;
}

bool IsUsernameExist(string Username, string FileName)
{
    fstream MyFile;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line = "";
        stUser User;

        while (getline(MyFile, Line))
        {
            User = ConvertDataLineToUserRecord(Line);

            if (User.UserName == Username)
            {
                MyFile.close();
                return true;
            }
        }

        MyFile.close();
    }
    return false;
}

stUser ReadNewUser()
{
    stUser User;

    cout << "\nPlease enter Username: ";
    getline(cin >> ws, User.UserName);

    while (IsUsernameExist(User.UserName, Users_FILE_NAME))
    {
        cout << "\nUsername [" << User.UserName << "], already exist! Please enter another Username: ";
        getline(cin, User.UserName);
    }

    cout << "Please enter Password: ";
    getline(cin, User.Password);

    User.Permissions = ReadPermissionsToSet();

    return User;
}

void AddNewUser()
{
    stUser User = ReadNewUser();

    AddDataLineToFile(Users_FILE_NAME, ConvertUserRecordToDataLine(User));
}

void AddNewUsers()
{
    char AddMore = 'n';

    do
    {
        cout << "\nAdding New User:\n\n";

        AddNewUser();

        cout << "\nUser Added Successfully! Do you want to add more users? y/n? ";
        cin >> AddMore;

    } while (tolower(AddMore) == 'y');
}

void ShowAddUsersScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tAdd New Users Screen\n";
    cout << "==================================\n";

    AddNewUsers();
}

string ReadUserName()
{
    string UserName;

    cout << "\nPlease enter UserName: ";
    getline(cin >> ws, UserName);

    return UserName;
}

bool FindUserByUserName(vector<stUser> vUsers, stUser &User, string UserName)
{
    for (stUser &U : vUsers)
    {
        if (U.UserName == UserName)
        {
            User = U;
            return true;
        }
    }

    return false;
}

void PrintUserCard(stUser User)
{
    cout << "\nThe followings are User Details:\n";
    cout << "----------------------------------------\n";
    cout << "User Name: " << User.UserName << endl;
    cout << "Password: " << User.Password << endl;
    cout << "Permissions: " << User.Permissions << endl;
    cout << "----------------------------------------\n";
}

void MarkUserForDelete(vector<stUser> &vUsers, string UserName)
{
    for (stUser &User : vUsers)
    {
        if (User.UserName == UserName)
        {
            User.MarkForDelete = true;
            break;
        }
    }
}

void SaveUsersToFile(vector<stUser> vUsers, string FileName)
{
    fstream MyFile;

    MyFile.open(FileName, ios::out);

    if (MyFile.is_open())
    {
        for (stUser &User : vUsers)
        {
            if (!User.MarkForDelete)
                MyFile << ConvertUserRecordToDataLine(User) << endl;
        }

        MyFile.close();
    }
}

void DeleteUserByUserName(vector<stUser> &vUsers, string UserName)
{
    stUser User;
    char Answer = 'n';

    if (FindUserByUserName(vUsers, User, UserName))
    {
        PrintUserCard(User);

        cout << "\nAre you sure, you want to delete this user? y/n? ";
        cin >> Answer;

        if (tolower(Answer) == 'y')
        {
            MarkUserForDelete(vUsers, UserName);
            SaveUsersToFile(vUsers, Users_FILE_NAME);

            cout << "\nClient Deleted Successfully!\n";
        }
    }
    else
        cout << "\nUser with UserName [" << UserName << "] is Not Found\n";
}

void ShowDeleteUserScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tDelete User Screen\n";
    cout << "==================================\n";

    vector<stUser> vUsers = LoadUsersDataFromFile(Users_FILE_NAME);
    string UserName = ReadUserName();

    if (UserName == "Admin")
        cout << "\nYou Can Not Delete this User!\n";
    else
        DeleteUserByUserName(vUsers, UserName);
}

stUser UpdateUser(string UserName)
{
    stUser User;

    User.UserName = UserName;

    cout << "\nPlease enter Password: ";
    getline(cin >> ws, User.Password);

    User.Permissions = ReadPermissionsToSet();

    return User;
}

void UpdateUserByUserName(vector<stUser> &vUsers, string UserName)
{
    stUser User;
    char Answer = 'n';

    if (FindUserByUserName(vUsers, User, UserName))
    {
        PrintUserCard(User);

        cout << "\nAre you sure, you want to update this user? y/n? ";
        cin >> Answer;

        if (tolower(Answer) == 'y')
        {
            for (stUser &U : vUsers)
            {
                if (U.UserName == UserName)
                {
                    U = UpdateUser(UserName);
                    break;
                }
            }
            SaveUsersToFile(vUsers, Users_FILE_NAME);

            cout << "\nClient Updated Successfully!\n";
        }
    }
    else
        cout << "\nUser with UserName [" << UserName << "] is Not Found\n";
}

void ShowUpdateUserScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tUpdate User Screen\n";
    cout << "==================================\n";

    vector<stUser> vUsers = LoadUsersDataFromFile(Users_FILE_NAME);
    string UserName = ReadUserName();

    if (UserName == "Admin")
        cout << "\nYou Can Not Update this User!\n";
    else
        UpdateUserByUserName(vUsers, UserName);
}

void ShowFindUserScreen()
{
    system("cls");
    cout << "==================================\n";
    cout << "\tFind User Screen\n";
    cout << "==================================\n";

    vector<stUser> vUsers = LoadUsersDataFromFile(Users_FILE_NAME);
    string UserName = ReadUserName();
    stUser User;

    if (FindUserByUserName(vUsers, User, UserName))
        PrintUserCard(User);
    else
        cout << "\nUser with UserName [" << UserName << "] is Not Found\n";
}

void PerformManageUsersMenuOption(enManageUsersMenuOptions ManageUsersOption)
{
    switch (ManageUsersOption)
    {
    case enManageUsersMenuOptions::eListUsers:
        ShowAllUsersScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenuOptions::eAddUsers:
        ShowAddUsersScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenuOptions::eDeleteUser:
        ShowDeleteUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenuOptions::eUpdateUser:
        ShowUpdateUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenuOptions::eFindUser:
        ShowFindUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenuOptions::eMainMenuScreen:
        GoBackToMainMenu();
        break;
    }
}

void ShowManageUsersMenu()
{
    system("cls");

    if (!CheckAccessPermission(enMainMenuPermissions::pManageUsers))
    {
        ShowAccessDeniedMessage();
        GoBackToMainMenu();
        return;
    }

    cout << "============================================\n";
    cout << "\tManage Users Menu Screen\n";
    cout << "============================================\n";
    cout << "\t[1] List Users.\n";
    cout << "\t[2] Add New User.\n";
    cout << "\t[3] Delete User.\n";
    cout << "\t[4] Update User.\n";
    cout << "\t[5] Find User.\n";
    cout << "\t[6] Main Menu.\n";
    cout << "============================================\n";

    PerformManageUsersMenuOption((enManageUsersMenuOptions)ReadManageUsersMenuOpetion());
}

short ReadMainMenuOpetion()
{
    short Option;

    do
    {
        cout << "Choose what you want to do? [1 to 8]? ";
        cin >> Option;

    } while (Option < 1 || Option > 8);

    return Option;
}

void PerformMainMenuOption(enMainMenuOptions MainMenuOption)
{
    switch (MainMenuOption)
    {
    case enMainMenuOptions::eListClients:
        ShowAllClientsScreen();
        GoBackToMainMenu();
        break;
    case enMainMenuOptions::eAddClients:
        ShowAddClientsScreen();
        GoBackToMainMenu();
        break;
    case enMainMenuOptions::eDeleteClient:
        ShowDeleteClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenuOptions::eUpdateClient:
        ShowUpdateClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenuOptions::eFindClient:
        ShowFindClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenuOptions::eTransactions:
        ShowTransactionsMenu();
        break;
    case enMainMenuOptions::eManageUsers:
        ShowManageUsersMenu();
        break;
    case enMainMenuOptions::eLogOut:
        Login();
        break;
    }
}

void ShowMainMenuScreen()
{
    system("cls");
    cout << "=================================================\n";
    cout << "\t\tMain Menu Sreen\n";
    cout << "=================================================\n";

    cout << "\t[1] Show Clients List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n";
    cout << "\t[4] Update Client Info.\n";
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n";
    cout << "\t[7] Manage Users.\n";
    cout << "\t[8] Logout.\n";
    cout << "=================================================\n";
    PerformMainMenuOption((enMainMenuOptions)ReadMainMenuOpetion());
}

bool FindUserByUserNameAndPassword(string Username, string Password, stUser &CurrentUser)
{
    vector<stUser> vUsers = LoadUsersDataFromFile(Users_FILE_NAME);

    for (stUser &User : vUsers)
    {
        if (User.UserName == Username && User.Password == Password)
        {
            CurrentUser = User;

            return true;
        }
    }

    return false;
}

bool LoadUserInfo(string Username, string Password)
{
    if (FindUserByUserNameAndPassword(Username, Password, CurrentUser))
        return true;
    else
        return false;
}

void Login()
{
    bool LoginFailed = false;
    string Username, Password;

    do
    {
        system("cls");
        cout << "==================================\n";
        cout << "\tLogin Screen\n";
        cout << "==================================\n";

        if (LoginFailed)
            cout << "\nInvalid Username/Password\n";

        cout << "\nEnter Username: ";
        cin >> Username;

        cout << "Enter Password: ";
        cin >> Password;

        LoginFailed = !LoadUserInfo(Username, Password);

    } while (LoginFailed);

    ShowMainMenuScreen();
}

int main()
{

    Login();

    system("pause>0");
    return 0;
}
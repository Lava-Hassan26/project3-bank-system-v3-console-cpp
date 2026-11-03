#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

const string clientsFileName = "Clients.txt";
const string usersFileName = "Users.txt";
const string delim = "#//#";

void MainMenuScreen();
void TransactionsMenuScreen();
bool IsClientExistInFile(string, string);
void ManageUsersMenuScreen();
bool IsUserExistInFile(string, string, string);
void Login();
bool IsUsernameExistInFile(string, string);
void DeniedScreen();

enum enMainMenu { ShowClientsList = 1, AddClient, DeleteClient, UpdateClientInfo, FindClient, Transactions, ManageUsers, Logout };

enum enTransactionsMenu { Deposit = 1, Withdraw, TotalBalances, MainMenu };

enum enManageUsersMenu { ShowUsersList = 1, AddUser, DeleteUser, UpdateUserInfo, FindUser, MainMenuUser };

enum enUserPermissions { eAll = -1, pShowClientsList = 1, pAddClient = 2, pDeleteClient = 4, pUpdateClientInfo = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

struct stClient
{
    string accountNumber = "";
    string pinCode = "";
    string fullName = "";
    string phoneNumber = "";
    double accountBalance = 0.0;
    bool isMarkedForDelete = false;
};

struct stUsers
{
    string username = "";
    string password = "";
    int userPermissions = 0;
    bool isMarkedForDelete = false;
};

stUsers currentUser;
bool CheckAccessPermission(enUserPermissions);

//Main Menu

string ReadString(string message)
{
    string text = "";
    cout << message;
    getline(cin >> ws, text);
    return text;
}

double ReadDoubleNumber(string message)
{
    double number = 0;
    cout << message;
    cin >> number;
    while (cin.fail())
    {
        cin.clear(); // resets the error state
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        cout << "INVALIDE! Enter a valid number: ";
        cin >> number;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return number;
}

string ReadAccountNumber()
{
    string accountNumber = "";
    accountNumber = ReadString("Enter the account number: ");
    return accountNumber;
}

stClient ReadClientInfo()
{
    stClient client;
    client.accountNumber = ReadString("\nEnter Account Number: ");

    while (IsClientExistInFile(client.accountNumber, clientsFileName))
    {
        cout << "\nClient with [" << client.accountNumber << "] already exists!\n";
        client.accountNumber = ReadString("\nEnter Account Number: ");
    }
    client.pinCode = ReadString("\nEnter Pin Code: ");
    client.fullName = ReadString("\nEnter Full Name: ");
    client.phoneNumber = ReadString("\nEnter Phone Number: ");
    client.accountBalance = ReadDoubleNumber("\nEnter Account Balance: ");

    return client;
}

stClient ReadUpdatedClientInfo(string accountNumber)
{
    stClient client;
    client.accountNumber = accountNumber;
    client.pinCode = ReadString("\nEnter Pin Code: ");
    client.fullName = ReadString("\nEnter Full Name: ");
    client.phoneNumber = ReadString("\nEnter Phone Number: ");
    client.accountBalance = ReadDoubleNumber("\nEnter Account Balance: ");

    return client;
}

enMainMenu ReadMainMenuUserChoice()
{
    short userChoice = 0;
    do
    {
        userChoice = ReadDoubleNumber("Enter your choice [1-8]: ");

    } while (userChoice < 1 || userChoice > 8);
    return (enMainMenu)userChoice;
}

enTransactionsMenu ReadTransactionsMenuUserChoice()
{
    short userChoice = 0;
    do
    {
        userChoice = ReadDoubleNumber("Enter your choice [1-4]: ");

    } while (userChoice < 1 || userChoice > 4);
    return (enTransactionsMenu)userChoice;
}

vector<string> SplitStringByDelimiter(string inputText, string delim = " ")
{
    vector<string> vWords;
    string word = "";
    short delimiterPosition = 0;
    delimiterPosition = inputText.find(delim);

    while ((delimiterPosition) != std::string::npos)
    {
        word = inputText.substr(0, delimiterPosition);
        if (word != "")
            vWords.push_back(word);
        inputText.erase(0, delimiterPosition + delim.length());
        delimiterPosition = inputText.find(delim);
    }
    if (inputText != "")
        vWords.push_back(inputText);
    return vWords;
}

string ConvertClientRecordToDataLine(stClient client, string delim)
{
    string line = "";
    line += client.accountNumber + delim;
    line += client.pinCode + delim;
    line += client.fullName + delim;
    line += client.phoneNumber + delim;
    line += to_string(client.accountBalance);
    return line;
}

stClient ConvertDataLineToClientRecord(string lineData, string delim)
{
    vector<string> vClient = SplitStringByDelimiter(lineData, delim);
    stClient client;
    client.accountNumber = vClient[0];
    client.pinCode = vClient[1];
    client.fullName = vClient[2];
    client.phoneNumber = vClient[3];
    client.accountBalance = stof(vClient[4]);

    return client;
}

void AddClientToFile(stClient client, string fileName, string delim)
{
    fstream clientsFile;
    clientsFile.open(fileName, ios::out | ios::app);
    if (clientsFile.is_open())
    {
        string line = ConvertClientRecordToDataLine(client, delim);
        clientsFile << line << endl;
        clientsFile.close();
    }
}

void SaveAllClientsToFile(vector<stClient>& vClients, string fileName, string delim)
{
    fstream clientsFile;
    clientsFile.open(fileName, ios::out);

    if (clientsFile.is_open())
    {
        string currentLline = "";
        for (stClient& client : vClients)
        {
            if (!client.isMarkedForDelete)
            {
                currentLline = ConvertClientRecordToDataLine(client, delim);
                clientsFile << currentLline << endl;
            }
        }
        clientsFile.close();
    }
}

vector<stClient> LoadAllClientsFromFile(string fileName, string delim)
{
    vector<stClient> vClients;
    fstream clientsFile;
    clientsFile.open(fileName, ios::in);

    if (clientsFile.is_open())
    {
        stClient client;
        string currentLline = "";
        while (getline(clientsFile, currentLline))
        {
            client = ConvertDataLineToClientRecord(currentLline, delim);
            vClients.push_back(client);
        }
        clientsFile.close();
    }
    return vClients;
}

bool IsClientExistInFile(string accountNumber, string fileName)
{
    vector <stClient> vClients;
    fstream clientsFile;
    clientsFile.open(fileName, ios::in);//read Mode
    if (clientsFile.is_open())
    {
        string Line;
        stClient Client;
        while (getline(clientsFile, Line))
        {
            Client = ConvertDataLineToClientRecord(Line, delim);
            if (Client.accountNumber == accountNumber)
            {
                clientsFile.close();
                return true;
            }
        }
        clientsFile.close();
    }
    return false;
}

bool IsClientExistInVector(string accountNumber, vector<stClient>& vClients, stClient& client)
{
    for (stClient& c : vClients)
    {
        if (c.accountNumber == accountNumber)
        {
            client = c;
            return true;
        }
    }
    return false;
}

bool MarkClientForDeletion(string accountNumber, vector<stClient>& vClients)
{
    for (stClient& c : vClients)
    {
        if (c.accountNumber == accountNumber)
        {
            c.isMarkedForDelete = true;
            return true;
        }
    }
    return false;
}

void PrintClientCardDetails(const stClient& client)
{
    cout << "\nThe Following are the Client Details";
    cout << "\n***************************************";
    cout << "\nAccount Number : " << client.accountNumber;
    cout << "\nPin Code       : " << client.pinCode;
    cout << "\nFull Name      : " << client.fullName;
    cout << "\nPhone Number   : " << client.phoneNumber;
    cout << "\nAccount Balance: " << client.accountBalance;
    cout << "\n***************************************\n\n";
}

void PrintClientsInfoHeader()
{
    cout << "\n__________________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(20) << "Account Number";
    cout << "| " << left << setw(15) << "Pin Code";
    cout << "| " << left << setw(25) << "Client Name";
    cout << "| " << left << setw(20) << "Phone Number";
    cout << "| " << left << setw(15) << "Account Balance";
    cout << "\n__________________________________________________________________________________________________________\n";
}

void ShowClientsListScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pShowClientsList))
    {
        DeniedScreen();
        return;
    }
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    cout << "\n\n\t\t\t\t\tList Of (" << vClients.size() << ") Clients\n";
    PrintClientsInfoHeader();

    for (stClient& client : vClients)
    {
        cout << "| " << left << setw(20) << client.accountNumber;
        cout << "| " << left << setw(15) << client.pinCode;
        cout << "| " << left << setw(25) << client.fullName;
        cout << "| " << left << setw(20) << client.phoneNumber;
        cout << "| " << left << setw(15) << client.accountBalance << endl;
    }
    cout << "__________________________________________________________________________________________________________\n\n";
}

void ShowClientDetailsByAccountNumber(string accountNumber)
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    stClient client;
    if (IsClientExistInVector(accountNumber, vClients, client))
        PrintClientCardDetails(client);
    else
        cout << "\nClient with account number " << accountNumber << " not found!\n\n";
}

void DeleteClientByAccountNumber(string accountNumber)
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    stClient client;
    char userChoice = 'y';
    if (IsClientExistInVector(accountNumber, vClients, client))
    {
        PrintClientCardDetails(client);
        cout << "Are you sure you want to delete this client? [y/n]? ";
        cin >> userChoice;
        if (tolower(userChoice) == 'y')
        {
            MarkClientForDeletion(accountNumber, vClients);
            SaveAllClientsToFile(vClients, clientsFileName, delim);
            cout << "\nClient Deleted successfully\n\n";
        }
        else
            cout << "\nDeletion Cancelled\n\n";
    }
    else
        cout << "\nClient with account number " << accountNumber << " not found!\n\n";
}

void UpdateClientByAccountNumber(string accountNumber)
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    stClient client;
    char userChoice = 'y';
    if (IsClientExistInVector(accountNumber, vClients, client))
    {
        PrintClientCardDetails(client);
        cout << "Are you sure you want to update this client? [y/n]? ";
        cin >> userChoice;
        if (tolower(userChoice) == 'y')
        {
            for (stClient& c : vClients)
            {
                if (c.accountNumber == accountNumber)
                {
                    c = ReadUpdatedClientInfo(accountNumber);
                    break;
                }
            }
            SaveAllClientsToFile(vClients, clientsFileName, delim);
            cout << "\nClient Updated successfully\n\n";
        }
        else
            cout << "\nUpdating Cancelled\n\n";
    }
    else
        cout << "\nClient with account number " << accountNumber << " not found!\n\n";
}

void AddNewClient()
{
    stClient client = ReadClientInfo();
    AddClientToFile(client, clientsFileName, delim);
}

void AddClients()
{
    char addMore = 'y';
    while (tolower(addMore) == 'y')
    {
        AddNewClient();
        cout << "\n\nClient added successfully\n";
        cout << "Do you want to add more client [Y/N]? ";
        cin >> addMore;
    }
}

void AddNewClientsScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pAddClient))
    {
        DeniedScreen();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n===========================================\n";
    AddClients();
}

void DeleteClientScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pDeleteClient))
    {
        DeniedScreen();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tDelete Client Screen";
    cout << "\n===========================================\n";
    DeleteClientByAccountNumber(ReadAccountNumber());
}

void UpdateClientScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pUpdateClientInfo))
    {
        DeniedScreen();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tUpdate Client Screen";
    cout << "\n===========================================\n";
    UpdateClientByAccountNumber(ReadAccountNumber());
}

void FindClientScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pFindClient))
    {
        DeniedScreen();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tFind Client Screen";
    cout << "\n===========================================\n";
    ShowClientDetailsByAccountNumber(ReadAccountNumber());
}

void ProgramEndsScreen()
{
    cout << "\n==========================================\n";
    cout << "\tProgram Ends Screen";
    cout << "\n==========================================\n";
}

void GoBackToMainMenu()
{
    cout << "Press any key to go back to main menu . . . ";
    system("pause>0");
    system("cls");
    MainMenuScreen();
}

void ExecuteMainMenu(enMainMenu userChoice)
{
    switch (userChoice)
    {
    case enMainMenu::ShowClientsList:
        system("cls");
        ShowClientsListScreen();
        GoBackToMainMenu();
        break;
    case enMainMenu::AddClient:
        system("cls");
        AddNewClientsScreen();
        GoBackToMainMenu();
        break;
    case enMainMenu::DeleteClient:
        system("cls");
        DeleteClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenu::UpdateClientInfo:
        system("cls");
        UpdateClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenu::FindClient:
        system("cls");
        FindClientScreen();
        GoBackToMainMenu();
        break;
    case enMainMenu::Transactions:
        system("cls");
        TransactionsMenuScreen();
        break;
    case enMainMenu::ManageUsers:
        system("cls");
        ManageUsersMenuScreen();
        break;
    case enMainMenu::Logout:
        system("cls");
        Login();
        break;
    }
}

void MainMenuScreen()
{
    system("cls");
    cout << "\n===========================================\n";
    cout << "\t\tMain Menu Screen";
    cout << "\n===========================================\n";
    cout << "\t[1] Show Clients List\n";
    cout << "\t[2] Add New Clients\n";
    cout << "\t[3] Delete Client\n";
    cout << "\t[4] Update Client Info\n";
    cout << "\t[5] Find Client\n";
    cout << "\t[6] Transactions\n";
    cout << "\t[7] Manage Users\n";
    cout << "\t[8] Logout";
    cout << "\n===========================================\n";
    ExecuteMainMenu(ReadMainMenuUserChoice());
}

//Transactions

void GoBackToTransactionsMenu()
{
    cout << "Press any key to go back to transactions menu . . . ";
    system("pause>0");
    system("cls");
    TransactionsMenuScreen();
}

void TotalBalancesListScreen()
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    cout << "\n\n\t\t\t\t\tList Of (" << vClients.size() << ") Balances Client\n";
    cout << "\n__________________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(20) << "Account Number";
    cout << "| " << left << setw(25) << "Client Name";
    cout << "| " << left << setw(15) << "Account Balance";
    cout << "\n__________________________________________________________________________________________________________\n";
    double totalBalances = 0.0;
    for (stClient& client : vClients)
    {
        cout << "| " << left << setw(20) << client.accountNumber;
        cout << "| " << left << setw(25) << client.fullName;
        cout << "| " << left << setw(15) << client.accountBalance << endl;
        totalBalances += client.accountBalance;
    }
    cout << "__________________________________________________________________________________________________________";
    cout << "\n\t\t\t\tTotal Balances= " << totalBalances << "\n\n";

}

void DepositAmount(double amount, string accountNumber, vector<stClient>& vClients)
{
    for (stClient& client : vClients)
    {
        if (client.accountNumber == accountNumber)
        {
            client.accountBalance += amount;
            break;
        }
    }
}

void WithdrawByAccountNumber(string accountNumber)
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    stClient client;
    char userChoice = 'y';

    while (!IsClientExistInVector(accountNumber, vClients, client))
    {
        cout << "\nClient with account number " << accountNumber << " not found!\n\n";
        accountNumber = ReadAccountNumber();
    }

    PrintClientCardDetails(client);
    double withdrawAmount = ReadDoubleNumber("Enter Withdraw Amount: ");

    while (withdrawAmount > client.accountBalance)
    {
        cout << "The Amount Exceeds The Balance, You Can Withdraw Up To " << client.accountBalance << endl;
        withdrawAmount = ReadDoubleNumber("Enter Withdraw Amount: ");
    }

    cout << "\nAre you sure you want to withdraw this amount? [y/n]? ";
    cin >> userChoice;

    if (tolower(userChoice) == 'y')
    {
        DepositAmount(-withdrawAmount, accountNumber, vClients);
        SaveAllClientsToFile(vClients, clientsFileName, delim);
        cout << "\nWithdraw Done successfully!\n\n";
    }
    else
        cout << "\nWithdraw Cancelled!\n\n";
}

void DepositByAccountNumber(string accountNumber)
{
    vector<stClient> vClients = LoadAllClientsFromFile(clientsFileName, delim);
    stClient client;
    char userChoice = 'y';

    while (!IsClientExistInVector(accountNumber, vClients, client))
    {
        cout << "\nClient with account number " << accountNumber << " not found!\n\n";
        accountNumber = ReadAccountNumber();
    }

    PrintClientCardDetails(client);
    double depositAmount = ReadDoubleNumber("Enter Deposit Amount: ");
    cout << "Are you sure you want to deposit this amount? [y/n]? ";
    cin >> userChoice;
    if (tolower(userChoice) == 'y')
    {
        DepositAmount(depositAmount, accountNumber, vClients);
        SaveAllClientsToFile(vClients, clientsFileName, delim);
        cout << "\nDeposit Done successfully!\n\n";
    }
    else
        cout << "\nDeposit Cancelled!\n\n";
}

void DepositScreen()
{
    cout << "\n===========================================\n";
    cout << "\t\tDeposit Screen";
    cout << "\n===========================================\n";
    DepositByAccountNumber(ReadAccountNumber());
}

void WithdrawScreen()
{
    cout << "\n===========================================\n";
    cout << "\t\tWithdraw Screen";
    cout << "\n===========================================\n";
    WithdrawByAccountNumber(ReadAccountNumber());
}

void ExecuteTransactionsMenu(enTransactionsMenu userChoice)
{
    switch (userChoice)
    {
    case enTransactionsMenu::Deposit:
        system("cls");
        DepositScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenu::Withdraw:
        system("cls");
        WithdrawScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenu::TotalBalances:
        system("cls");
        TotalBalancesListScreen();
        GoBackToTransactionsMenu();
        break;
    case enTransactionsMenu::MainMenu:
        system("cls");
        MainMenuScreen();
        break;
    default:
        system("cls");
        TransactionsMenuScreen();
    }
}

void TransactionsMenuScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pTransactions))
    {
        DeniedScreen();
        GoBackToMainMenu();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tTransactions Menu Screen";
    cout << "\n===========================================\n";
    cout << "\t[1] Deposit\n";
    cout << "\t[2] Withdraw\n";
    cout << "\t[3] Total Balances\n";
    cout << "\t[4] Main Menu";
    cout << "\n===========================================\n";
    ExecuteTransactionsMenu(ReadTransactionsMenuUserChoice());
}

//Users 

enManageUsersMenu ReadManageUsersMenuUserChoice()
{
    short userChoice = 0;
    do
    {
        userChoice = ReadDoubleNumber("Enter your choice [1-6]: ");

    } while (userChoice < 1 || userChoice > 6);
    return (enManageUsersMenu)userChoice;
}

string ReadUsername()
{
    string username = "";
    username = ReadString("Enter Username: ");
    return username;
}

string ReadPassword()
{
    string password = "";
    password = ReadString("Enter Password: ");
    return password;
}

stUsers ReadUsernameAndPassword()
{
    stUsers user;
    user.username = ReadUsername();
    user.password = ReadPassword();
    return user;
}

int ReadPermissions()
{
    int permissions = 0;
    char answer = 'n';
    cout << "\nDo you want to give full access? [Y/N]? ";
    cin >> answer;
    if (tolower(answer) == 'y')
        return -1;
    else
    {
        cout << "\nDo you want to give access to:\n";
        cout << "\nShow Clients List? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pShowClientsList;

        cout << "\nAdd New Clients? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pAddClient;

        cout << "\nDelete Client? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pDeleteClient;

        cout << "\nUpdate Client? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pUpdateClientInfo;

        cout << "\nFind Client? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pFindClient;

        cout << "\nTransactions? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pTransactions;

        cout << "\nManage Users? [Y/N]? ";
        cin >> answer;
        if (tolower(answer) == 'y')
            permissions += enUserPermissions::pManageUsers;
    }
    return permissions;

}

stUsers ReadUserInfo()
{
    stUsers user;
    user.username = ReadUsername();
    while (IsUsernameExistInFile(user.username, usersFileName))
    {
        cout << "\nUser With [" << user.username << "] Username Already Exist!\n";
        user.username = ReadUsername();
    }
    user.password = ReadPassword();
    user.userPermissions = ReadPermissions();
    return user;
}

stUsers ReadUpdatedUserInfo(stUsers& user)
{
    user.username = user.username;
    user.password = ReadPassword();
    user.userPermissions = ReadPermissions();
    return user;
}

string ConvertUserRecordToDataLine(stUsers user, string delim)
{
    string line = "";
    line += user.username + delim;
    line += user.password + delim;
    line += to_string(user.userPermissions);
    return line;
}

stUsers ConvertDataLineToUserRecord(string DataLine, string delim)
{
    vector<string> vUser = SplitStringByDelimiter(DataLine, delim);
    stUsers user;
    user.username = vUser[0];
    user.password = vUser[1];
    user.userPermissions = stoi(vUser[2]);
    return user;
}

void AddUserToFile(stUsers user, string fileName, string delim)
{
    fstream usersFile;
    usersFile.open(fileName, ios::out | ios::app);
    if (usersFile.is_open())
    {
        string line = ConvertUserRecordToDataLine(user, delim);
        usersFile << line << endl;
        usersFile.close();
    }
}

void SaveAllUsersToFile(vector<stUsers>& vUsers, string fileName, string delim)
{
    fstream usersFile;
    usersFile.open(fileName, ios::out);

    if (usersFile.is_open())
    {
        string currentLline = "";
        for (stUsers& user : vUsers)
        {
            if (!user.isMarkedForDelete)
            {
                currentLline = ConvertUserRecordToDataLine(user, delim);
                usersFile << currentLline << endl;
            }
        }
        usersFile.close();
    }
}

vector<stUsers> LoadAllUsersFromFile(string fileName, string delim)
{
    vector<stUsers> vUsers;
    fstream usersFile;
    usersFile.open(fileName, ios::in);
    if (usersFile.is_open())
    {
        stUsers user;
        string currentLline = "";
        while (getline(usersFile, currentLline))
        {
            user = ConvertDataLineToUserRecord(currentLline, delim);
            vUsers.push_back(user);
        }
        usersFile.close();
    }
    return vUsers;
}

bool IsUserExistInFile(string username, string password, stUsers& user, string fileName)
{
    vector <stUsers> vUsers;
    fstream usersFile;
    usersFile.open(fileName, ios::in);//read Mode
    if (usersFile.is_open())
    {
        string Line;
        stUsers fileUser;
        while (getline(usersFile, Line))
        {
            fileUser = ConvertDataLineToUserRecord(Line, delim);
            if (fileUser.username == username && fileUser.password == password)
            {
                user = fileUser;
                usersFile.close();
                return true;
            }
        }
        usersFile.close();
    }
    return false;
}

bool IsUsernameExistInFile(string username, string fileName)
{
    vector <stUsers> vUsers;
    fstream usersFile;
    usersFile.open(fileName, ios::in);//read Mode
    if (usersFile.is_open())
    {
        string Line;
        stUsers fileUser;
        while (getline(usersFile, Line))
        {
            fileUser = ConvertDataLineToUserRecord(Line, delim);
            if (fileUser.username == username)
            {
                usersFile.close();
                return true;
            }
        }
        usersFile.close();
    }
    return false;
}

bool IsUsernameExistInVector(stUsers& user, vector<stUsers>& vUsers)
{
    for (stUsers& u : vUsers)
    {
        if (u.username == user.username)
        {
            user = u;
            return true;
        }
    }
    return false;
}

bool MarkUserForDeletion(stUsers user, vector<stUsers>& vUsers)
{
    for (stUsers& u : vUsers)
    {
        if (u.username == user.username && u.password == user.password)
        {
            u.isMarkedForDelete = true;
            return true;
        }
    }
    return false;
}

void PrintUserCardDetails(const stUsers& user)
{
    cout << "\nThe Following are the User Details";
    cout << "\n***************************************";
    cout << "\nUsername   : " << user.username;
    cout << "\nPassword   : " << user.password;
    cout << "\nPermissions: " << user.userPermissions;
    cout << "\n***************************************\n\n";
}

void PrintUsersInfoHeader()
{
    cout << "\n__________________________________________________________________________________________________________\n\n";
    cout << "| " << left << setw(20) << "UserName";
    cout << "| " << left << setw(20) << "Password";
    cout << "| " << left << setw(25) << "Permissions";
    cout << "\n__________________________________________________________________________________________________________\n";
}

void ShowUsersListScreen()
{
    vector<stUsers> vUsers = LoadAllUsersFromFile(usersFileName, delim);
    cout << "\n\n\t\t\t\t\tList Of (" << vUsers.size() << ") Users\n";
    PrintUsersInfoHeader();

    for (stUsers& user : vUsers)
    {
        cout << "| " << left << setw(20) << user.username;
        cout << "| " << left << setw(20) << user.password;
        cout << "| " << left << setw(25) << user.userPermissions << endl;
    }
    cout << "__________________________________________________________________________________________________________\n\n";
}

void ShowUserDetailsByUsername(stUsers& user)
{
    vector<stUsers> VUsers = LoadAllUsersFromFile(usersFileName, delim);
    if (IsUsernameExistInVector(user, VUsers))
        PrintUserCardDetails(user);
    else
        cout << "\nInvalid Username/Password!\n\n";
}

void DeleteUserByUsername(stUsers& user)
{
    vector<stUsers> vUsers = LoadAllUsersFromFile(usersFileName, delim);
    char userChoice = 'y';
    if (IsUsernameExistInVector(user, vUsers))
    {
        PrintUserCardDetails(user);
        cout << "Are you sure you want to delete this user? [y/n]? ";
        cin >> userChoice;
        if (tolower(userChoice) == 'y')
        {
            MarkUserForDeletion(user, vUsers);
            SaveAllUsersToFile(vUsers, usersFileName, delim);
            cout << "\nUser Deleted successfully\n\n";
        }
        else
            cout << "\nDeletion Cancelled\n\n";
    }
    else
        cout << "\nUser with username " << user.username << " not found!\n\n";
}

void UpdateUserByUsername(stUsers& user)
{
    vector<stUsers> vUsers = LoadAllUsersFromFile(usersFileName, delim);
    char userChoice = 'y';
    if (IsUsernameExistInVector(user, vUsers))
    {
        PrintUserCardDetails(user);
        cout << "Are you sure you want to update this user? [y/n]? ";
        cin >> userChoice;
        if (tolower(userChoice) == 'y')
        {
            for (stUsers& u : vUsers)
            {
                if (u.username == user.username && u.password == user.password)
                {
                    u = ReadUpdatedUserInfo(user);
                    break;
                }
            }
            SaveAllUsersToFile(vUsers, usersFileName, delim);
            cout << "\nUser Updated successfully\n\n";
        }
        else
            cout << "\nUpdating Cancelled\n\n";
    }
    else
        cout << "\nUser with username " << user.username << " not found!\n\n";
}

void AddNewUser()
{
    stUsers user = ReadUserInfo();
    AddUserToFile(user, usersFileName,delim);
}

void AddUsers()
{
    char addMore = 'y';
    while (tolower(addMore) == 'y')
    {
        AddNewUser();
        cout << "\n\nUser added successfully\n";
        cout << "Do you want to add more Users [Y/N]? ";
        cin >> addMore;
    }
}

void AddNewUsersScreen()
{
    cout << "\n===========================================\n";
    cout << "\tAdd New Users Screen";
    cout << "\n===========================================\n";
    AddUsers();
}

void DeleteUserScreen()
{
    cout << "\n===========================================\n";
    cout << "\tDelete User Screen";
    cout << "\n===========================================\n";
    stUsers user;
    user.username = ReadUsername();
    DeleteUserByUsername(user);
}

void UpdateUserScreen()
{
    cout << "\n===========================================\n";
    cout << "\tUpdate User Screen";
    cout << "\n===========================================\n";
    stUsers user;
    user.username = ReadUsername();
    UpdateUserByUsername(user);
}

void FindUserScreen()
{
    cout << "\n===========================================\n";
    cout << "\tFind User Screen";
    cout << "\n===========================================\n";
    stUsers user;
    user.username = ReadUsername();
    ShowUserDetailsByUsername(user);
}

void GoBackToManageUsersMenu()
{
    cout << "Press any key to go back to manage users menu . . . ";
    system("pause>0");
    system("cls");
    ManageUsersMenuScreen();
}

void ExecuteManageUsersMenu(enManageUsersMenu userChoise)
{
    switch (userChoise)
    {
    case enManageUsersMenu::ShowUsersList:
        system("cls");
        ShowUsersListScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenu::AddUser:
        system("cls");
        AddNewUsersScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenu::DeleteUser:
        system("cls");
        DeleteUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenu::UpdateUserInfo:
        system("cls");
        UpdateUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenu::FindUser:
        system("cls");
        FindUserScreen();
        GoBackToManageUsersMenu();
        break;
    case enManageUsersMenu::MainMenuUser:
        system("cls");
        MainMenuScreen();
        break;
    }
}

void ManageUsersMenuScreen()
{
    if (!CheckAccessPermission(enUserPermissions::pManageUsers))
    {
        DeniedScreen();
        GoBackToMainMenu();
        return;
    }
    cout << "\n===========================================\n";
    cout << "\tManage Users Menu Screen";
    cout << "\n===========================================\n";
    cout << "\t[1] Show Users List\n";
    cout << "\t[2] Add New User\n";
    cout << "\t[3] Delete User\n";
    cout << "\t[4] Update User Info\n";
    cout << "\t[5] Find User\n";
    cout << "\t[6] Main Menu";
    cout << "\n===========================================\n";
    ExecuteManageUsersMenu(ReadManageUsersMenuUserChoice());
}

void DeniedScreen()
{
    system("cls");
    cout << "\n===========================================\n";
    cout << "Access Denied!\n";
    cout << "You Do Not Have Permission To Do This Action!\n";
    cout << "Contact Your Admin For Help\n";
    cout << "\n===========================================\n";
}

bool CheckAccessPermission(enUserPermissions permission)
{
    if (currentUser.userPermissions == enUserPermissions::eAll)
        return true;
    if ((permission & currentUser.userPermissions) == permission)
        return true;
    else
        return false;
}

void LoginScreen()
{
    system("cls");
    cout << "\n===========================================\n";
    cout << "\t\tLogin Screen";
    cout << "\n===========================================\n";
}

bool LoadCurrentUserInfo(string username, string password)
{
    if (IsUserExistInFile(username, password, currentUser, usersFileName))
        return true;
    else
        return false;
}

void Login()
{
    bool isLoginFaild = false;
    string username = "", password = "";
    do
    {
        LoginScreen();
        if (isLoginFaild)
            cout << "Invalid Username/Password!\n";

        username = ReadUsername();
        password = ReadPassword();

        isLoginFaild = !LoadCurrentUserInfo(username, password);

    } while (isLoginFaild);
    MainMenuScreen();
}

int main()
{
    Login();

    return 0;
}
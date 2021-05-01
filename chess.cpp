#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

/*------------------------------Structure------------------------------*/

typedef pair<int, int> ii;

struct Move {
    ii fromPos;
    int fromPiece;
    ii toPos;
    int toPiece;

    void setMove(ii _fromPos, int _fromPiece, ii _toPos, int _toPiece) {
        fromPos = _fromPos;
        fromPiece = _fromPiece;
        toPos = _toPos;
        toPiece = _toPiece;
    }
};

struct Button {
    sf::Text text;
    ii position;
    ii size;

    void setButton(sf::Font &font,string textString, int textSize, ii buttonPosition, ii buttonSize) {
        text.setFont(font);
        text.setString(textString);
        text.setCharacterSize(textSize);
        text.setFillColor(sf::Color::Black);
        text.setPosition(buttonPosition.second, buttonPosition.first);
        position = buttonPosition;
        size = buttonSize;
    }
};

/*------------------------------Val------------------------------*/

//Texture
sf::Texture boardTexture;
sf::Texture pieceTexture[13];

//Sprite
sf::Sprite boardSprite;
sf::Sprite pieceSprite[8][8];
sf::CircleShape pMoveShape[8][8];

//Font
sf::Font font;

//Game Logic
int startBoard[8][8] = {
    -5, -4, -3, -2, -1, -3, -4, -5,
    -6, -6, -6, -6, -6, -6, -6, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 4, 3, 2, 1, 3, 4, 5
};
int mainBoard[8][8];

ii directVerHor[4] = {ii(0, 1), ii(0, -1), ii(1, 0), ii(-1, 0)};
ii directDia[4] = {ii(1, 1), ii(1, -1), ii(-1, 1), ii(-1, -1)};
ii directL[8] = {ii(2, 1), ii(2, -1), ii(-2, 1), ii(-2, -1), ii(1, 2), ii(1, -2), ii(-1, 2), ii(-1, -2)};
int pieceValue[7] = {0, 10000, 9, 3, 3, 5, 1};

//Scene Control
int scene;
ii mouseClickPosition;
ii lastPos, currentPos;
int lastPiece, currentPiece;

string player1;
string player2;
int playerSide = 0;
int botLevel = 5;
int turn;

vector<Move> moveList;
bool pMove[8][8];
Move bestMove;

/*------------------------------Load Resource------------------------------*/

void loadFont() {
    font.loadFromFile("font/monoton.ttf");
}

void loadBoardTextureAndSprite() {
    boardTexture.loadFromFile("image/board.png");
    boardTexture.setSmooth(true);

    boardSprite.setTexture(boardTexture);
    boardSprite.setScale(0.5, 0.5);
}

void loadPieceTexture() {
    pieceTexture[6].loadFromFile("image/blank.png");

    pieceTexture[7].loadFromFile("image/white_king.png");
    pieceTexture[8].loadFromFile("image/white_queen.png");
    pieceTexture[9].loadFromFile("image/white_bishop.png");
    pieceTexture[10].loadFromFile("image/white_knight.png");
    pieceTexture[11].loadFromFile("image/white_rook.png");
    pieceTexture[12].loadFromFile("image/white_pawn.png");
    
    pieceTexture[5].loadFromFile("image/black_king.png");
    pieceTexture[4].loadFromFile("image/black_queen.png");
    pieceTexture[3].loadFromFile("image/black_bishop.png");
    pieceTexture[2].loadFromFile("image/black_knight.png");
    pieceTexture[1].loadFromFile("image/black_rook.png");
    pieceTexture[0].loadFromFile("image/black_pawn.png");

    for (int i = 0; i <= 12; ++i) 
        pieceTexture[i].setSmooth(true);

    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
        pieceSprite[i][j].setPosition(j * 75, i * 75);
        pieceSprite[i][j].setScale(0.5, 0.5);

        pMoveShape[i][j].setRadius(12.5);
        pMoveShape[i][j].setPosition(j * 75 + 25, i * 75 + 25);
        pMoveShape[i][j].setFillColor(sf::Color(0, 0, 0, 60));
    }
}

/*------------------------------Mouse Detech------------------------------*/

bool isMouseOnButton(Button &button) {
    int x = button.position.first;
    int y = button.position.second;

    int x_ = x + button.size.first;
    int y_ = y + button.size.second;

    int mx = mouseClickPosition.first;
    int my = mouseClickPosition.second;

    if (x <= mx && mx < x_ && y <= my && my < y_) return true;
    return false;
}

ii mouseOnPiece() {
    int mx = mouseClickPosition.first;
    int my = mouseClickPosition.second;

    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
        if (i * 75 <= mx && mx < (i + 1) * 75)
        if (j * 75 <= my && my < (j + 1) * 75) return ii(i, j);
    }

    return ii(-1, -1); 
}

/*------------------------------Game Logic------------------------------*/

void initBoard(string _player1, string _player2) {
    turn = 0;
    player1 = _player1;
    player2 = _player2;

    lastPiece = 11;
    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) 
        mainBoard[i][j] = startBoard[i][j];
}

void possibleMoveStraight(vector<ii>& pMove, ii pos, int piece, int ndirect, ii* direct, int distance) {
    for (int i = 0; i < ndirect; ++i) {
        int x = pos.first;
        int y = pos.second;

        for (int j = 1; j <= distance; ++j) {
            x += direct[i].first;
            y += direct[i].second;

            if (x < 0 || x >= 8 || y < 0 || y >= 8) break;
            
            int p = mainBoard[x][y];
            if (p == 0) {
                pMove.push_back(ii(x, y));
                continue;
            } 
            if (p * piece > 0) {
                break;
            }
            if (p * piece < 0) {
                pMove.push_back(ii(x, y));
                break;
            }
        }
    }
}

void possibleMovePawn(vector<ii>& pMove, ii pos, int piece) {
    int x = pos.first;
    int y = pos.second;

    int base, direct;
    if (piece < 0) {
        base = 1;
        direct = 1;
    }
    else {
        base = 6;
        direct = -1;
    }

    if (x < 7 && mainBoard[x + direct][y] == 0) 
        pMove.push_back(ii(x + direct, y));

    if (x == base && mainBoard[x + direct][y] == 0 && mainBoard[x + direct * 2][y] == 0) 
        pMove.push_back(ii(x + direct * 2, y));

    if (y > 0 && x < 7 && mainBoard[x + direct][y - 1] * piece < 0) 
        pMove.push_back(ii(x + direct, y - 1));

    if (y < 7 && x < 7 && mainBoard[x + direct][y + 1] * piece < 0) 
        pMove.push_back(ii(x + direct, y + 1));
}

void possibleMove(vector<ii>& pMove, ii pos, int piece) {
    if (abs(piece) == 1) {
        possibleMoveStraight(pMove, pos, piece, 4, directVerHor, 1);
        possibleMoveStraight(pMove, pos, piece, 4, directDia, 1);
    }

    if (abs(piece) == 2) {
        possibleMoveStraight(pMove, pos, piece, 4, directVerHor, 8);
        possibleMoveStraight(pMove, pos, piece, 4, directDia, 8);
    }

    if (abs(piece) == 3) {
        possibleMoveStraight(pMove, pos, piece, 4, directDia, 8);
    }

    if (abs(piece) == 4) {
        possibleMoveStraight(pMove, pos, piece, 8, directL, 1);
    }

    if (abs(piece) == 5) {
        possibleMoveStraight(pMove, pos, piece, 4, directVerHor, 8);
    }

    if (abs(piece) == 6) {
        possibleMovePawn(pMove, pos, piece);
    }
}

int value(int piece) {
    return (piece >= 0) ? pieceValue[piece] : -pieceValue[-piece];
}

int costMove() {
    int res = 0;
    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
        res += value(mainBoard[i][j]);
    return res;
}


void move(ii pos1, ii pos2) {  
    Move w;
    w.setMove(pos1, mainBoard[pos1.first][pos1.second], pos2, mainBoard[pos2.first][pos2.second]);
    moveList.push_back(w);

    mainBoard[pos2.first][pos2.second] = mainBoard[pos1.first][pos1.second];;
    mainBoard[pos1.first][pos1.second] = 0;

    turn = 1 - turn;
}

void undo() {
    Move w = moveList[moveList.size() - 1];
    moveList.pop_back();

    ii pos1 = w.fromPos;
    ii pos2 = w.toPos;

    mainBoard[pos1.first][pos1.second] = w.fromPiece;
    mainBoard[pos2.first][pos2.second] = w.toPiece;

    turn = 1 - turn;
}

int Alpha_Beta(int depth, int cost, int alpha, int beta) {
    if (depth == 0) return cost;

    if (turn == 0) {
        for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (mainBoard[i][j] <= 0) continue;

            vector<ii> _pMove;
            possibleMove(_pMove, ii(i, j), mainBoard[i][j]);
            for (auto p: _pMove) {
                int tmp = value(mainBoard[p.first][p.second]);
                move(ii(i, j), p);
                int res = Alpha_Beta(depth - 1, cost - tmp, alpha, beta);
                undo();
                
                //if (depth == botLevel && res == alpha && rand() % 32768 < 5000) bestMove.setMove(ii(i,j), mainBoard[i][j], p, mainBoard[p.first][p.second]);
                if (depth == botLevel && res > alpha) bestMove.setMove(ii(i,j), mainBoard[i][j], p, mainBoard[p.first][p.second]);

                alpha = max(alpha, res);
                if (alpha >= beta) return alpha;
            }
        }
        return alpha;
    } 
    else {
        for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (mainBoard[i][j] >= 0) continue;

            vector<ii> _pMove;
            possibleMove(_pMove, ii(i, j), mainBoard[i][j]);
            for (auto p: _pMove) {
                int tmp = value(mainBoard[p.first][p.second]);
                move(ii(i, j), p);
                int res = Alpha_Beta(depth - 1, cost - tmp, alpha, beta);
                undo();

                //if (depth == botLevel && res == beta && rand() % 32768 < 5000) bestMove.setMove(ii(i,j), mainBoard[i][j], p, mainBoard[p.first][p.second]);
                if (depth == botLevel && res < beta) bestMove.setMove(ii(i,j), mainBoard[i][j], p, mainBoard[p.first][p.second]);

                beta = min(beta, res);
                if (alpha >= beta) return beta;
            }
        }
        return beta;
    }
}

int checkWin() {
    int res = 0;
    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
        if (abs(mainBoard[i][j]) == 1) res += mainBoard[i][j];

    return res;
}

void play() {
    string currentPlayer = (turn == 0) ? player1 : player2;
    //cout << "__________" << currentPiece << " " << currentPos.first << " " << currentPos.second << endl;

    if (currentPlayer == "player") {
        if (mouseClickPosition == ii(-1, -1)) return;
        currentPos = mouseOnPiece();
        currentPiece = mainBoard[currentPos.first][currentPos.second];


        if ((currentPiece > 0 && turn == 0) || (currentPiece < 0 && turn == 1)) {
            lastPos = currentPos;
            lastPiece = currentPiece;
            vector<ii> _pMove;
            possibleMove(_pMove, currentPos, currentPiece);
            memset(pMove, false, sizeof(pMove));
            for (auto p : _pMove)
                pMove[p.first][p.second] = true;
        }
        else {
            cout << turn << " " << lastPiece << " " << lastPos.first << " " << lastPos.second << " - " << currentPiece << " " << currentPos.first << " " << currentPos.second << endl;
            if (lastPiece != 11 && pMove[currentPos.first][currentPos.second]) {
                move(lastPos, currentPos);
                cout << turn << " " << lastPiece << " " << lastPos.first << " " << lastPos.second << " - " << currentPiece << " " << currentPos.first << " " << currentPos.second << endl;
                lastPiece = currentPiece;
                lastPos = currentPos;
                memset(pMove, false, sizeof(pMove));                    
            }
            else {
                lastPiece = 11;
                lastPos = ii(-1, -1);
                memset(pMove, false, sizeof(pMove));
            }
        }
    } 
    else {
        cout << "-- BOT START --" << endl;
        Alpha_Beta(botLevel, costMove(), -100000, 100000);
        cout << bestMove.fromPos.first << " " << bestMove.fromPos.second << " **** " << bestMove.toPos.first << " " << bestMove.toPos.second << endl; 
        move(bestMove.fromPos, bestMove.toPos);
        lastPos = bestMove.toPos;
        lastPiece = mainBoard[bestMove.toPos.first][bestMove.toPos.second];
        memset(pMove, false, sizeof(pMove));
        cout << "-- BOT FINISH --" << endl;
    }
    //cout << turn << " " << lastPiece << " " << lastPos.first << " " << lastPos.second << " - " << currentPiece << " " << currentPos.first << " " << currentPos.second << endl;
}

/*------------------------------Display Component------------------------------*/

void displayButton(sf::RenderWindow &window, Button &button) {
    window.draw(button.text);
}

void displayChessBoard(sf::RenderWindow &window) {
    window.draw(boardSprite);

    if (lastPiece != 11) {
        sf::RectangleShape square(sf::Vector2f(75, 75));
        square.setPosition(lastPos.second * 75, lastPos.first * 75);
        square.setFillColor(sf::Color(0, 0, 0, 40));
        window.draw(square);
    }

    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
        if (mainBoard[i][j] != 0) {
            pieceSprite[i][j].setTexture(pieceTexture[mainBoard[i][j] + 6]);
            window.draw(pieceSprite[i][j]);
        }

        if (pMove[i][j]) {
            window.draw(pMoveShape[i][j]);
        }
    }
}

/*------------------------------Scene------------------------------*/

void displayMenuScene(sf::RenderWindow &window) {
    window.draw(boardSprite);


    Button pvbButton;
    pvbButton.setButton(font, "Player - Bot", 45, ii(255, 0), ii(45, 375));
    displayButton(window, pvbButton);
    if (isMouseOnButton(pvbButton)) scene = 2;


    Button pvpButton;
    pvpButton.setButton(font, "Player - Player", 45, ii(330, 0), ii(45, 480));
    displayButton(window, pvpButton);
    if (isMouseOnButton(pvpButton)) scene = 3;


    Button settingButton;
    settingButton.setButton(font, "Setting", 45, ii(405, 0), ii(45, 195));
    displayButton(window, settingButton);
    if (isMouseOnButton(settingButton)) scene = 5;


    Button quitButton;
    quitButton.setButton(font, "Quit", 45, ii(480, 0), ii(45, 120));
    displayButton(window, quitButton);
    if (isMouseOnButton(quitButton)) window.close();
}


void displayPlayerVsBotScene(sf::RenderWindow &window) {
    window.draw(boardSprite);


    Button backButton;
    backButton.setButton(font, "<", 45, ii(180, 0), ii(45, 40));
    displayButton(window, backButton);
    if (isMouseOnButton(backButton)) scene = 1;


    Button playButton;
    playButton.setButton(font, "Play", 45, ii(180, 150), ii(45, 150));
    displayButton(window, playButton);
    if (isMouseOnButton(playButton)) {
        if (playerSide == 0) initBoard("player", "bot");
            else initBoard("bot", "player");
        scene = 4;   
    }


    Button playerSideText;
    playerSideText.setButton(font, "Side", 45, ii(255, 150), ii(0, 0));
    displayButton(window, playerSideText);


    Button playerSideButton;
    playerSideButton.setButton(font, (playerSide == 0) ? "white" : "black", 45, ii(255, 375), ii(45, 150));
    displayButton(window, playerSideButton);
    if (isMouseOnButton(playerSideButton)) playerSide = 1 - playerSide;


    Button levelText;
    levelText.setButton(font, "Level", 45, ii(330, 150), ii(0, 0));
    displayButton(window, levelText);


    Button levelNumText;
    levelNumText.setButton(font, to_string(botLevel), 45, ii(330, 405), ii(0, 0));
    displayButton(window, levelNumText);


    Button botLevelUpButton;
    botLevelUpButton.setButton(font, "+", 45, ii(330, 480), ii(45, 45));
    displayButton(window, botLevelUpButton);
    if (isMouseOnButton(botLevelUpButton) && botLevel < 8) botLevel++;


    Button botLevelDownButton;
    botLevelDownButton.setButton(font, "-", 45, ii(330, 335), ii(45, 45));
    displayButton(window, botLevelDownButton);
    if (isMouseOnButton(botLevelDownButton) && botLevel > 1) botLevel--;
}

void displayPlayerVsPlayerScene(sf::RenderWindow &window) {
    window.draw(boardSprite); 


    Button backButton;
    backButton.setButton(font, "<", 45, ii(180, 0), ii(45, 40));
    displayButton(window, backButton);
    if (isMouseOnButton(backButton)) scene = 1;


    Button playButton;
    playButton.setButton(font, "Play", 45, ii(180, 150), ii(45, 150));
    displayButton(window, playButton);
    if (isMouseOnButton(playButton)) {
        initBoard("player", "player");
        scene = 4;   
    }
}

void displayGameScene(sf::RenderWindow &window) {
    int tmp = checkWin();
    if (tmp == 0) {
        play();
        displayChessBoard(window);
    }
    else {
        displayChessBoard(window);

        sf::RectangleShape square(sf::Vector2f(600, 600));
        square.setFillColor(sf::Color(0, 0, 0, 40));
        window.draw(square);


        Button winButton;
        winButton.setButton(font, (tmp == 1) ? "white win!" : "black win", 45, ii(255, 150), ii(45, 260));
        displayButton(window, winButton);
        if (isMouseOnButton(winButton)) scene = 1;
    }
}

void displaySettingScene(sf::RenderWindow &window) {
    window.draw(boardSprite);


    Button backButton;
    backButton.setButton(font, "<", 45, ii(180, 0), ii(45, 40));
    displayButton(window, backButton);
    if (isMouseOnButton(backButton)) scene = 1;
}

/*------------------------------Main------------------------------*/

int main()
{   
    srand(time(NULL));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    // Window
    sf::RenderWindow window(sf::VideoMode(600, 600), "GNUD CHESS", sf::Style::Default, settings);

    //Start Up Initialize
    loadFont();
    loadBoardTextureAndSprite();
    loadPieceTexture();
    scene = 1;

    while (window.isOpen()) {
        sf::Event event;
        mouseClickPosition = ii(-1, -1);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonReleased)
                if (event.mouseButton.button == sf::Mouse::Left) 
                    mouseClickPosition = ii(event.mouseButton.y, event.mouseButton.x);
        }

        window.clear();
        switch (scene) {
            case 1:
                displayMenuScene(window);
                break;
            case 2:
                displayPlayerVsBotScene(window);
                break;
            case 3:
                displayPlayerVsPlayerScene(window);
                break;
            case 4:
                displayGameScene(window);
                break;
            case 5:
                displaySettingScene(window);
                break;
        }
        window.display();
    }

    return 0;
}
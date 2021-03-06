// C++HESS.
// (C)Igor Lobanov. 2017
//
// This is a cpp file.
#include "StrategyManual.h"
#include "Player.h"

#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "King.h"

// Manual game behavior
bool StrategyManual::play( std::shared_ptr<Board> board_, const Player* player, int32_t& i,
                     int32_t& j, int32_t& i2, int32_t& j2 ) const{

  const int32_t BSIZE = BoardGlobals::getSize();
  char j_ = '\0', j2_= '\0';
  char ch = '\0';
  char i_ = '\0', i2_ = '\0';
  bool isKingUnderAttack = false;

  std::cout << "\n  " << player->getColor() << "S turn:";

  while ( true ){

    while( _kbhit() )
      _getch();

    // To allow left game before each turn.
    std::cout << "\n  Press ANY KEY to proceed, ESC to exit to main menu: ";
    ch = _getch();
    if( 27 == ch )
      return false;

    // Read next turn coordinates.
    std::cout << "\n  Enter current-next piece coordinates, e.g. e2-e4: ";
    std::cin >> j_ >> i_ >> ch >> j2_ >> i2_;

    // Remove all characters from cin.
    std::cin.ignore( 100, '\n' );

    // Convert chars into 2D array coordinates.
    i  = i_  - 48          - 1;
    j  = 97  - j_  + BSIZE - 1;
    i2 = i2_ - 48          - 1;
    j2 = 97  - j2_ + BSIZE - 1;

    // Corner / border cases.
    if ( BSIZE <= j  ||
         BSIZE <= j2 ||
         BSIZE <= i  ||
         BSIZE <= i2 ||
         0 > j  ||
         0 > j2 ||
         0 > i  ||
         0 > i2 )
          continue;

    Piece*& currentSquarePiece = board_->modify()[i][j]; // Reference to a pointer
    // If piece exists and player color corresponds to piece color.
    if( currentSquarePiece &&
        currentSquarePiece->getPieceColor() == player->getColor() ){

      bool isMoved = currentSquarePiece->movePiece( board_,
                                                    player,
                                                    currentSquarePiece,
                                                    i2,
                                                    j2 );
      if( isMoved )
        return true;
      else
        isMoved = currentSquarePiece->killPiece( board_,
                                                 player,
                                                 currentSquarePiece,
                                                 i2,
                                                 j2,
                                                 isKingUnderAttack );

      if( isMoved )
        return true; ; // Pass move to next piece.
    }
  }

  return false;
}



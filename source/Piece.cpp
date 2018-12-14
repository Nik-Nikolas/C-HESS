// C++HESS.
// (C)Igor Lobanov. 2017
//
// This is a cpp file.
#include "Player.h"
#include "Piece.h"

void Piece::replacePiece( Board* board,
                          const Player* player,
                          Piece*& newPiece,
                          Piece*& oldPiece,
                          const int32_t i,
                          const int32_t j ){

  // Old piece pointer now points to new piece.
  delete oldPiece;
  oldPiece = newPiece;
  newPiece->markAsMoved();

  // Save last move data: piece type, color, turn and coordinates.
  board->cacheLastMovedPiece( newPiece->getPieceType(),
                              PieceCoordinates ( i,
                              'A' + j ),
                              newPiece->getPieceColor(),
                              player->getGame()->currentTurn() );

  newPiece = nullptr;
}



bool Piece::killPiece( Board* board,
                       const Player* player,
                       Piece*& piece,
                       const int32_t i,
                       const int32_t j,
                       bool& isKingUnderAttack ){

  isKingUnderAttack = false;

  Piece*& enemySquare =
  board->changeBoard()[i][j];

  if( nullptr == enemySquare ){
    // Check for empty square.
    return false;
  }
  else if( enemySquare->getPieceColor() == piece->getPieceColor() ){
    // Check for proper (enemy) color.
    return false;
  }
  else if( typeid( *enemySquare ) == typeid( King ) ){
    // Enemy king is under attack.
    isKingUnderAttack = true;
    // Enemy king is immortal.
    return false;
  }
  else{

    replacePiece( board, player, piece, enemySquare, i, j );

    // Pass next move to opponent.
    return true;
  }
}



bool Piece::movePiece( Board* board,
                       const Player* player,
                       Piece*& piece,
                       const int32_t i,
                       const int32_t j ){

  // If no coordinates were changed or square is busy - treat next piece.
  Piece*& boardSquare =
  board->changeBoard()[i][j];
  if( boardSquare ){
    return false;
  }
  else {
    // Move piece.
    boardSquare = piece;
    piece->markAsMoved();
    piece = nullptr;

    // Save last move data: piece type, color, turn and coordinates.
    board->cacheLastMovedPiece( boardSquare->getPieceType(),
                                PieceCoordinates ( i,
                                'A' + j ),
                                boardSquare->getPieceColor(),
                                player->getGame()->currentTurn() );

    // Pass next move to opponent.
    return true;
  }
}



bool Piece::killForwardLine( Board* board,
                             const Player* player,
                             Piece*& piece,
                             const int32_t i,
                             const int32_t j,
                             int32_t& i2,
                             int32_t& j2,
                             bool& isKingUnderAttack ){

  // Kill forward enemy in the any forward enemy Square.
  bool foundForwardEnemy = false;
  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;
  if( piece->getPieceColor() == PieceColor::WHITE && i < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( forwardEnemySquare_y < BoardGlobals::getSize() - 1 ){
      ++forwardEnemySquare_y;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece == white - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 ){

    while( forwardEnemySquare_y > 0 ){
      --forwardEnemySquare_y;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece - black - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundForwardEnemy )
    // isKingUnderAttack - fact of was an enemy king found or was not.
    if ( piece->killPiece( board, player, piece, forwardEnemySquare_y,
                           forwardEnemySquare_x, isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = forwardEnemySquare_y;
      j2 = forwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same
                // color piece - treat next piece.
}



bool Piece::killBackwardLine( Board* board,
                               const Player* player,
                               Piece*& piece,
                               const int32_t i,
                               const int32_t j,
                               int32_t& i2,
                               int32_t& j2,
                               bool& isKingUnderAttack ){

  // Kill backward enemy in the any backward enemy Square.
  bool foundBackwardEnemy = false;
  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;
  if( piece->getPieceColor() == PieceColor::BLACK &&
      i < BoardGlobals::getSize() - 1 ){ // BLACKS.

    while( backwardEnemySquare_y < BoardGlobals::getSize() - 1){
      ++backwardEnemySquare_y;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest forward piece == same color - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundBackwardEnemy = true;
          break;
        }
      }
    }
  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && i > 0 ){

    while( backwardEnemySquare_y > 0 ){
      --backwardEnemySquare_y;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest forward piece - same color - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundBackwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundBackwardEnemy )
    if ( piece->killPiece( board,
                           player,
                           piece,
                           backwardEnemySquare_y,
                           backwardEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = backwardEnemySquare_y;
      j2 = backwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::killLeftLine( Board* board,
                           const Player* player,
                           Piece*& piece,
                           const int32_t i,
                           const int32_t j,
                           int32_t& i2,
                           int32_t& j2,
                           bool& isKingUnderAttack ){

  // Kill left enemy in the any left enemy Square.
  bool foundLeftEnemy = false;
  int32_t leftEnemySquare_x = j;
  int32_t leftEnemySquare_y = i;
  if( piece->getPieceColor() == PieceColor::WHITE &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( leftEnemySquare_x < BoardGlobals::getSize() - 1 ){
      ++leftEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[leftEnemySquare_y][leftEnemySquare_x];
      // If nearest left piece == same color - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == this->getPieceColor() )
          return false;
        else{
          foundLeftEnemy = true;
          break;
        }
      }
    }
  }// BLACKS.

  else if( piece->getPieceColor() == PieceColor::BLACK && j > 0 ){
    while( leftEnemySquare_x > 0 ){
      --leftEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[leftEnemySquare_y][leftEnemySquare_x];
      // If nearest left piece - same color - do nothing.
      if( potentialPiece ){
          if( potentialPiece->getPieceColor() == this->getPieceColor() )
            return false;
          else{
            foundLeftEnemy = true;
            break;
        }
      }
    }
  }

  if( foundLeftEnemy )
    if( piece->killPiece( board,
                          player,
                          piece,
                          leftEnemySquare_y,
                          leftEnemySquare_x,
                          isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = leftEnemySquare_y;
      j2 = leftEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::killRightLine( Board* board,
                           const Player* player,
                           Piece*& piece,
                           const int32_t i,
                           const int32_t j,
                           int32_t& i2,
                           int32_t& j2,
                           bool& isKingUnderAttack ){

  // Kill right enemy in the any right enemy Square.
  bool foundRightEnemy = false;
  int32_t rightEnemySquare_x = j;
  int32_t rightEnemySquare_y = i;
  if( piece->getPieceColor() == PieceColor::BLACK && j < BoardGlobals::getSize() - 1){ // BLACKS.

    while( rightEnemySquare_x < BoardGlobals::getSize() - 1 ){
      ++rightEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[rightEnemySquare_y][rightEnemySquare_x];
      // If nearest right piece == same color - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundRightEnemy = true;
          break;
        }
      }
    }
  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && j > 0 ){

    while( rightEnemySquare_x > 0 ){
      --rightEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[rightEnemySquare_y][rightEnemySquare_x];
      // If nearest right piece - same color - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundRightEnemy = true;
          break;
        }
      }
    }
  }

  if( foundRightEnemy )
    if ( piece->killPiece( board,
                           player,
                           piece,
                           rightEnemySquare_y,
                           rightEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = rightEnemySquare_y;
      j2 = rightEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::moveForwardLine2( Board* board,
                              const Player* player,
                              Piece*& piece,
                              const int32_t i,
                              const int32_t j,
                              int32_t& i2,
                              int32_t& j2,
                              bool& isKingUnderAttack,
                              const int32_t LENGTH ){

  int32_t forwardSquare_x = j;
  int32_t forwardSquare_y = i;
  int32_t squareIter = 0;

  if( piece->getPieceColor() == PieceColor::WHITE && i < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( forwardSquare_y < BoardGlobals::getSize() - 1 &&
           squareIter < LENGTH ){
      ++forwardSquare_y;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[forwardSquare_y][forwardSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){
        --forwardSquare_y;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 ){

    while( forwardSquare_y > 0 &&
           squareIter < LENGTH ){
      --forwardSquare_y;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[forwardSquare_y][forwardSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){
        ++forwardSquare_y;
        break;
      }
    }
  }

  if ( piece->movePiece( board,
                         player,
                         piece,
                         forwardSquare_y,
                         forwardSquare_x ) ){

    // Coords of new piece square.
    i2 = forwardSquare_y;
    j2 = forwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy by king - treat next piece.
}



bool Piece::moveBackwardLine2( Board* board,
                               const Player* player,
                               Piece*& piece,
                               const int32_t i,
                               const int32_t j,
                               int32_t& i2,
                               int32_t& j2,
                               bool& isKingUnderAttack,
                               const int32_t LENGTH ){

  int32_t backwardSquare_x = j;
  int32_t backwardSquare_y = i;
  int32_t squareIter = 0;

  if( piece->getPieceColor() == PieceColor::BLACK &&
      i < BoardGlobals::getSize() - 1 ){ // BLACKS.


    while( backwardSquare_y < BoardGlobals::getSize() - 1 &&
           squareIter < LENGTH ){
      ++backwardSquare_y;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[backwardSquare_y][backwardSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){
        --backwardSquare_y;
        break;
      }
    }
  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && i > 0 ){

    while( backwardSquare_y > 0 &&
           squareIter < LENGTH ){
      --backwardSquare_y;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[backwardSquare_y][backwardSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){

        ++backwardSquare_y;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, backwardSquare_y, backwardSquare_x ) ){

    // Coords of new piece square.
    i2 = backwardSquare_y;
    j2 = backwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}


bool Piece::moveLeftLine2( Board* board,
                           const Player* player,
                           Piece*& piece,
                           const int32_t i,
                           const int32_t j,
                           int32_t& i2,
                           int32_t& j2,
                           bool& isKingUnderAttack,
                           const int32_t LENGTH  ){

  int32_t leftSquare_x = j;
  int32_t leftSquare_y = i;
  int32_t squareIter = 0;

  if( piece->getPieceColor() == PieceColor::WHITE && j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( leftSquare_x < BoardGlobals::getSize() - 1 &&
           squareIter < LENGTH ){
      ++leftSquare_x;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[leftSquare_y][leftSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare){
        --leftSquare_x;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && j > 0 ){

    while( leftSquare_x > 0  &&
           squareIter < LENGTH ){
      --leftSquare_x;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[leftSquare_y][leftSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){
        ++leftSquare_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, leftSquare_y, leftSquare_x ) ){

    // Coords of new piece square.
    i2 = leftSquare_y;
    j2 = leftSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveRightLine2( Board* board,
                           const Player* player,
                           Piece*& piece,
                           const int32_t i,
                           const int32_t j,
                           int32_t& i2,
                           int32_t& j2,
                           bool& isKingUnderAttack,
                           const int32_t LENGTH ){

  int32_t rightSquare_x = j;
  int32_t rightSquare_y = i;
  int32_t squareIter = 0;

  if( piece->getPieceColor() == PieceColor::BLACK &&
      j < BoardGlobals::getSize() - 1 ){ // BLACKS.

    while( rightSquare_x < BoardGlobals::getSize() - 1 &&
           squareIter < LENGTH ){
      ++rightSquare_x;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[rightSquare_y][rightSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare){
        --rightSquare_x;
        break;
      }
    }
  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && j > 0 ){

    while( rightSquare_x > 0  &&
           squareIter < LENGTH ){
      --rightSquare_x;
      ++squareIter;

      Piece* potentialSquare =
      board->changeBoard()[rightSquare_y][rightSquare_x];
      // If nearest Square is busy - break cycle.
      if( potentialSquare ){
        ++rightSquare_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, rightSquare_y, rightSquare_x ) ){

    // Coords of new piece square.
    i2 = rightSquare_y;
    j2 = rightSquare_x;

    return true; // Pass next move to opponent.
  }
  else
      return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveForwardSquare( Board* board,
                               const Player* player,
                               Piece*& piece,
                               const int32_t i,
                               const int32_t j,
                               int32_t& i2,
                               int32_t& j2,
                               bool& isKingUnderAttack ){

  int32_t forwardSquare_x = j;
  int32_t forwardSquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && i < BoardGlobals::getSize() - 1){ // WHITES.

    ++forwardSquare_y;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 ){

    --forwardSquare_y;

  }

  if ( piece->movePiece( board, player, piece, forwardSquare_y, forwardSquare_x ) ){

    // Coords of new piece square.
    i2 = forwardSquare_y;
    j2 = forwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
      return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::killForwardSquare( Board* board,
                               const Player* player,
                               Piece*& piece,
                               const int32_t i,
                               const int32_t j,
                               int32_t& i2,
                               int32_t& j2,
                               bool& isKingUnderAttack ){

  int32_t forwardSquare_x = j;
  int32_t forwardSquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && i < BoardGlobals::getSize() - 1){ // WHITES.

    ++forwardSquare_y;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 ){

    --forwardSquare_y;

  }

  if ( piece->killPiece( board, player, piece, forwardSquare_y, forwardSquare_x,
                         isKingUnderAttack ) ){

    // Coords of enemy piece.
    i2 = forwardSquare_y;
    j2 = forwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}

bool Piece::moveBackwardSquare( Board* board,
                                const Player* player,
                                Piece*& piece,
                                const int32_t i,
                                const int32_t j,
                                int32_t& i2,
                                int32_t& j2,
                                bool& isKingUnderAttack ){

  int32_t backwardSquare_x = j;
  int32_t backwardSquare_y = i;

  if( piece->getPieceColor() == PieceColor::BLACK && i < BoardGlobals::getSize() - 1){ // BLACKS.

    ++backwardSquare_y;

  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && i > 0 ){

    --backwardSquare_y;

  }

  if ( piece->movePiece( board, player, piece, backwardSquare_y, backwardSquare_x ) ){

    // Coords of new piece square.
    i2 = backwardSquare_y;
    j2 = backwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
      return false; // If no coordinates were changed or square is busy - treat next piece.
}

bool Piece::killBackwardSquare( Board* board,
                                const Player* player,
                                Piece*& piece,
                                const int32_t i,
                                const int32_t j,
                                int32_t& i2,
                                int32_t& j2,
                                bool& isKingUnderAttack ){

  int32_t backwardSquare_x = j;
  int32_t backwardSquare_y = i;

  if( piece->getPieceColor() == PieceColor::BLACK && i < BoardGlobals::getSize() - 1){ // BLACKS.

    ++backwardSquare_y;

  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && i > 0 ){

    --backwardSquare_y;

  }

  if ( piece->killPiece( board, player, piece, backwardSquare_y, backwardSquare_x,
                         isKingUnderAttack ) ){

    // Coords of enemy piece.
    i2 = backwardSquare_y;
    j2 = backwardSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveLeftSquare( Board* board,
                            const Player* player,
                            Piece*& piece,
                            const int32_t i,
                            const int32_t j,
                            int32_t& i2,
                            int32_t& j2,
                            bool& isKingUnderAttack ){

  int32_t leftSquare_x = j;
  int32_t leftSquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && j < BoardGlobals::getSize() - 1){ // WHITES.

    ++leftSquare_x;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && j > 0 ){

    --leftSquare_x;

  }

  if ( piece->movePiece( board, player, piece, leftSquare_y, leftSquare_x ) ){

    // Coords of new piece square.
    i2 = leftSquare_y;
    j2 = leftSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::killLeftSquare( Board* board,
                            const Player* player,
                            Piece*& piece,
                            const int32_t i,
                            const int32_t j,
                            int32_t& i2,
                            int32_t& j2,
                            bool& isKingUnderAttack ){

  int32_t leftSquare_x = j;
  int32_t leftSquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && j < BoardGlobals::getSize() - 1){ // WHITES.

    ++leftSquare_x;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && j > 0 ){

    --leftSquare_x;

  }

  if ( piece->killPiece( board, player, piece, leftSquare_y, leftSquare_x,
                         isKingUnderAttack ) ){

    // Coords of enemy piece.
    i2 = leftSquare_y;
    j2 = leftSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveRightSquare( Board* board,
                             const Player* player,
                             Piece*& piece,
                             const int32_t i,
                             const int32_t j,
                             int32_t& i2,
                             int32_t& j2,
                             bool& isKingUnderAttack ){

  int32_t rightSquare_x = j;
  int32_t rightSquare_y = i;

  if( piece->getPieceColor() == PieceColor::BLACK &&
      j < BoardGlobals::getSize() - 1 ){ // BLACKS.

    ++rightSquare_x;

  } // WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && j > 0 ){

    --rightSquare_x;

  }

  if ( piece->movePiece( board, player, piece, rightSquare_y, rightSquare_x ) ){

    // Coords of new piece square.
    i2 = rightSquare_y;
    j2 = rightSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::killRightSquare( Board* board,
                             const Player* player,
                             Piece*& piece,
                             const int32_t i,
                             const int32_t j,
                             int32_t& i2,
                             int32_t& j2,
                             bool& isKingUnderAttack ){

  int32_t rightSquare_x = j;
  int32_t rightSquare_y = i;

  if( piece->getPieceColor() == PieceColor::BLACK &&
      j < BoardGlobals::getSize() - 1 ){ // BLACKS.

    ++rightSquare_x;

  }// WHITES.
  else if( piece->getPieceColor() == PieceColor::WHITE && j > 0 ){

    --rightSquare_x;

  }

  if ( piece->killPiece( board, player, piece, rightSquare_y, rightSquare_x,
                         isKingUnderAttack ) ){

    // Coords of enemy piece.
    i2 = rightSquare_y;
    j2 = rightSquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::movePerpendicularSquare( Board* board,
                                     const Player* player,
                                     Piece*& piece,
                                     const int32_t i,
                                     const int32_t j,
                                     int32_t& i2,
                                     int32_t& j2,
                                     bool& isKingUnderAttack ){

  const int32_t PERPENDICULAR_DIRECTIONS_MAX_AMT = 4;
  const int32_t MOVE = rand() % PERPENDICULAR_DIRECTIONS_MAX_AMT;
  const int32_t LENGTH = 1;

  if ( 0 == MOVE )
    if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;

  if ( 1 == MOVE )
    if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;

  if ( 2 == MOVE )
    if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;

  if ( 3 == MOVE )
    if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack , LENGTH ) )
      return true;

  return false; // No more moves available. Pass next move to OPPONENT piece.
}



bool Piece::movePerpendicularLine( Board* board,
                               const Player* player,
                               Piece*& piece,
                               const int32_t i,
                               const int32_t j,
                               int32_t& i2,
                               int32_t& j2,
                               bool& isKingUnderAttack ){

  const int32_t PERPENDICULAR_DIRECTIONS_MAX_AMT = 4;
  const int32_t MOVE = rand() % PERPENDICULAR_DIRECTIONS_MAX_AMT;
  const int32_t LENGTH = rand() % std::min( BoardGlobals::getSize(), BoardGlobals::getSize() ) + 1;

  if ( 0 == MOVE )
    if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 1 == MOVE )
    if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 2 == MOVE )
    if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 3 == MOVE )
    if( moveLeftLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveForwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveRightLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveBackwardLine2( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  return false; // No more moves available. Pass next move to OPPONENT piece.
}



bool Piece::moveDiagonalSquare( Board* board,
                                const Player* player,
                                Piece*& piece,
                                const int32_t i,
                                const int32_t j,
                                int32_t& i2,
                                int32_t& j2,
                                bool& isKingUnderAttack ){

  const int32_t DIAGONAL_DIRECTIONS_MAX_AMT = 4;
  const int32_t MOVE = rand() % DIAGONAL_DIRECTIONS_MAX_AMT;
  const int32_t LENGTH = 1;

  if ( 0 == MOVE )
    if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 1 == MOVE )
    if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 2 == MOVE )
    if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 3 == MOVE )
    if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  return false; // No more moves available. Pass next move to OPPONENT piece.
}



bool Piece::moveDiagonalLine( Board* board,
                              const Player* player,
                              Piece*& piece,
                              const int32_t i,
                              const int32_t j,
                              int32_t& i2,
                              int32_t& j2,
                              bool& isKingUnderAttack ){

  const int32_t DIAGONAL_DIRECTIONS_MAX_AMT = 4;
  const int32_t MOVE = rand() % DIAGONAL_DIRECTIONS_MAX_AMT;
  const int32_t LENGTH = rand() % std::min( BoardGlobals::getSize(), BoardGlobals::getSize() ) + 1;

  if ( 0 == MOVE )
    if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 1 == MOVE )
    if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 2 == MOVE )
    if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  if ( 3 == MOVE )
    if( moveDiagonalBackwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalForwardRightLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;
    else if( moveDiagonalBackwardLeftLine( board, player, piece, i, j, i2, j2, isKingUnderAttack, LENGTH ) )
      return true;

  return false; // No more moves available. Pass next move to OPPONENT piece.
}



bool Piece::killDiagonalForwardLeftLine( Board* board,
                                         const Player* player,
                                         Piece*& piece,
                                         const int32_t i,
                                         const int32_t j,
                                         int32_t& i2,
                                         int32_t& j2,
                                         bool& isKingUnderAttack ){

  // Kill forward enemy in the any forward enemy Square.
  bool foundForwardEnemy = false;
  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( forwardEnemySquare_y < BoardGlobals::getSize() - 1 &&
           forwardEnemySquare_x < BoardGlobals::getSize() - 1){
      ++forwardEnemySquare_y;
      ++forwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece == white - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 && j > 0){

    while( forwardEnemySquare_y > 0 && forwardEnemySquare_x > 0){
      --forwardEnemySquare_y;
      --forwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece - black - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundForwardEnemy )
    if ( piece->killPiece( board,
                           player,
                           piece,
                           forwardEnemySquare_y,
                           forwardEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = forwardEnemySquare_y;
      j2 = forwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::killDiagonalBackwardRightLine( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack ){

  // Kill backward enemy in the any backward enemy Square.
  bool foundBackwardEnemy = false;
  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && i > 0 && j > 0 ){ // WHITES.

    while( backwardEnemySquare_y > 0 && backwardEnemySquare_x > 0 ){
      --backwardEnemySquare_y;
      --backwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest backward piece == white - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundBackwardEnemy = true;
          break;
        }
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1 &&
           j < BoardGlobals::getSize() - 1 ){

    while( backwardEnemySquare_y < BoardGlobals::getSize() - 1 &&
           backwardEnemySquare_x < BoardGlobals::getSize() - 1 ){
      ++backwardEnemySquare_y;
      ++backwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest backward piece - black - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundBackwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundBackwardEnemy )
    if ( piece->killPiece( board, player, piece,
                           backwardEnemySquare_y,
                           backwardEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = backwardEnemySquare_y;
      j2 = backwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::killDiagonalForwardRightLine( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack ){

  // Kill forward enemy in the any forward enemy Square.
  bool foundForwardEnemy = false;
  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j > 0 ){ // WHITES.

    while( forwardEnemySquare_y < BoardGlobals::getSize() - 1 &&
           forwardEnemySquare_x > 0 ){
      ++forwardEnemySquare_y;
      --forwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece == white - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 &&
           j < BoardGlobals::getSize() - 1 ){

    while( forwardEnemySquare_y > 0 && forwardEnemySquare_x < BoardGlobals::getSize() - 1 ){
      --forwardEnemySquare_y;
      ++forwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest forward piece - black - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundForwardEnemy )
    if ( piece->killPiece( board, player, piece,
                           forwardEnemySquare_y,
                           forwardEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = forwardEnemySquare_y;
      j2 = forwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}




bool Piece::killDiagonalBackwardLeftLine( Board* board,
                                         const Player* player,
                                         Piece*& piece,
                                         const int32_t i,
                                         const int32_t j,
                                         int32_t& i2,
                                         int32_t& j2,
                                         bool& isKingUnderAttack ){

  // Kill backward enemy in the any backward enemy Square.
  bool foundForwardEnemy = false;
  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i > 0 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( backwardEnemySquare_y > 0 &&
           backwardEnemySquare_x < BoardGlobals::getSize() - 1 ){
      --backwardEnemySquare_y;
      ++backwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest backward piece == white - exit.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::WHITE )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::BLACK ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i < BoardGlobals::getSize() - 1  &&
           j > 0  ){

    while( backwardEnemySquare_y < BoardGlobals::getSize() - 1 && backwardEnemySquare_x > 0 ){
      ++backwardEnemySquare_y;
      --backwardEnemySquare_x;

      Piece* potentialPiece =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest backward piece - black - do nothing.
      if( potentialPiece ){
        if( potentialPiece->getPieceColor() == PieceColor::BLACK )
          return false;

        if ( potentialPiece->getPieceColor() == PieceColor::WHITE ){
          foundForwardEnemy = true;
          break;
        }
      }
    }
  }

  if( foundForwardEnemy )
    if ( piece->killPiece( board, player, piece,
                           backwardEnemySquare_y,
                           backwardEnemySquare_x,
                           isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = backwardEnemySquare_y;
      j2 = backwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }

  return false; // Square is the same / empty / busy by opposite king or same color piece - treat next piece.
}



bool Piece::moveDiagonalForwardLeftSquare( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack ){

  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    ++forwardEnemySquare_y;
    ++forwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 && j > 0 ){

    --forwardEnemySquare_y;
    --forwardEnemySquare_x;

  }

  if ( piece->movePiece( board, player, piece, forwardEnemySquare_y, forwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = forwardEnemySquare_y;
    j2 = forwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}


bool Piece::killDiagonalForwardLeftSquare( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack ){

  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    ++forwardEnemySquare_y;
    ++forwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK && i > 0 && j > 0){

    --forwardEnemySquare_y;
    --forwardEnemySquare_x;

  }

  if ( piece->killPiece( board,
                         player,
                         piece,
                         forwardEnemySquare_y,
                         forwardEnemySquare_x,
                         isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = forwardEnemySquare_y;
      j2 = forwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalBackwardRightSquare( Board* board,
                                             const Player* player,
                                             Piece*& piece,
                                             const int32_t i,
                                             const int32_t j,
                                             int32_t& i2,
                                             int32_t& j2,
                                             bool& isKingUnderAttack ){

  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE && i > 0 && j > 0 ){ // WHITES.

    --backwardEnemySquare_y;
    --backwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1 &&
           j < BoardGlobals::getSize() - 1 ){

    ++backwardEnemySquare_y;
    ++backwardEnemySquare_x;

  }

  if ( piece->movePiece( board, player, piece, backwardEnemySquare_y, backwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = backwardEnemySquare_y;
    j2 = backwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
      return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::killDiagonalBackwardRightSquare( Board* board,
                                             const Player* player,
                                             Piece*& piece,
                                             const int32_t i,
                                             const int32_t j,
                                             int32_t& i2,
                                             int32_t& j2,
                                             bool& isKingUnderAttack ){

  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;
  if( piece->getPieceColor() == PieceColor::WHITE && i > 0 && j > 0 ){ // WHITES.

    --backwardEnemySquare_y;
    --backwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1 &&
           j < BoardGlobals::getSize() - 1 ){

    ++backwardEnemySquare_y;
    ++backwardEnemySquare_x;

  }

  if ( piece->killPiece( board,
                         player,
                         piece,
                         backwardEnemySquare_y,
                         backwardEnemySquare_x,
                         isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = backwardEnemySquare_y;
      j2 = backwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalForwardRightSquare( Board* board,
                                            const Player* player,
                                            Piece*& piece,
                                            const int32_t i,
                                            const int32_t j,
                                            int32_t& i2,
                                            int32_t& j2,
                                            bool& isKingUnderAttack ){

  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j > 0 ){ // WHITES.

    ++forwardEnemySquare_y;
    --forwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i > 0 &&
           j < BoardGlobals::getSize() - 1 ){

    --forwardEnemySquare_y;
    ++forwardEnemySquare_x;

  }

  if ( piece->movePiece( board, player, piece, forwardEnemySquare_y, forwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = forwardEnemySquare_y;
    j2 = forwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
      return false; // If no coordinates were changed or square is busy - treat next piece.
}


bool Piece::killDiagonalForwardRightSquare( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack ){

  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j > 0 ){ // WHITES.

    ++forwardEnemySquare_y;
    --forwardEnemySquare_x;

  } // BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i > 0 &&
           j < BoardGlobals::getSize() - 1 ){

    --forwardEnemySquare_y;
    ++forwardEnemySquare_x;

  }

  if ( piece->killPiece( board,
                         player, piece,
                         forwardEnemySquare_y,
                         forwardEnemySquare_x,
                         isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = forwardEnemySquare_y;
      j2 = forwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalBackwardLeftSquare( Board* board,
                                            const Player* player,
                                            Piece*& piece,
                                            const int32_t i,
                                            const int32_t j,
                                            int32_t& i2,
                                            int32_t& j2,
                                            bool& isKingUnderAttack ){

  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i > 0 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

      --backwardEnemySquare_y;
      ++backwardEnemySquare_x;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1  &&
           j > 0  ){

      ++backwardEnemySquare_y;
      --backwardEnemySquare_x;

  }

  if ( piece->movePiece( board, player, piece, backwardEnemySquare_y, backwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = backwardEnemySquare_y;
    j2 = backwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}


bool Piece::killDiagonalBackwardLeftSquare( Board* board,
                                             const Player* player,
                                             Piece*& piece,
                                             const int32_t i,
                                             const int32_t j,
                                             int32_t& i2,
                                             int32_t& j2,
                                             bool& isKingUnderAttack ){

  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i > 0 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

      --backwardEnemySquare_y;
      ++backwardEnemySquare_x;

  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1  &&
           j > 0  ){

      ++backwardEnemySquare_y;
      --backwardEnemySquare_x;

  }

  if ( piece->killPiece( board,
                         player,
                         piece,
                         backwardEnemySquare_y,
                         backwardEnemySquare_x,
                         isKingUnderAttack ) ){

      // Coords of enemy piece.
      i2 = backwardEnemySquare_y;
      j2 = backwardEnemySquare_x;

      return true; // Pass next move to opponent.
    }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalForwardLeftLine( Board* board,
                                         const Player* player,
                                         Piece*& piece,
                                         const int32_t i,
                                         const int32_t j,
                                         int32_t& i2,
                                         int32_t& j2,
                                         bool& isKingUnderAttack,
                                         const int32_t LENGTH ){

  int32_t forwardEnemySquare_x = j;
  int32_t forwardEnemySquare_y = i;
  int32_t lengthIter = 0;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( forwardEnemySquare_y < BoardGlobals::getSize() - 1 &&
           forwardEnemySquare_x < BoardGlobals::getSize() - 1 &&
           lengthIter < LENGTH ){
      ++forwardEnemySquare_y;
      ++forwardEnemySquare_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        --forwardEnemySquare_y;
        --forwardEnemySquare_x;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i > 0 &&
           j > 0){

    while( forwardEnemySquare_y > 0 &&
           forwardEnemySquare_x > 0 &&
           lengthIter < LENGTH ){
      --forwardEnemySquare_y;
      --forwardEnemySquare_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[forwardEnemySquare_y][forwardEnemySquare_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        ++forwardEnemySquare_y;
        ++forwardEnemySquare_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, forwardEnemySquare_y, forwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = forwardEnemySquare_y;
    j2 = forwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalBackwardRightLine( Board* board,
                                           const Player* player,
                                           Piece*& piece,
                                           const int32_t i,
                                           const int32_t j,
                                           int32_t& i2,
                                           int32_t& j2,
                                           bool& isKingUnderAttack,
                                           const int32_t LENGTH ){

  int32_t backwardEnemySquare_x = j;
  int32_t backwardEnemySquare_y = i;
  int32_t lengthIter = 0;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i > 0 &&
      j > 0 ){ // WHITES.

    while( backwardEnemySquare_y > 0 &&
           backwardEnemySquare_x > 0 &&
           lengthIter < LENGTH ){
      --backwardEnemySquare_y;
      --backwardEnemySquare_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        ++backwardEnemySquare_y;
        ++backwardEnemySquare_x;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1 &&
           j < BoardGlobals::getSize() - 1 ){

    while( backwardEnemySquare_y < BoardGlobals::getSize() - 1 &&
           backwardEnemySquare_x < BoardGlobals::getSize() - 1 &&
           lengthIter < LENGTH ){
      ++backwardEnemySquare_y;
      ++backwardEnemySquare_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[backwardEnemySquare_y][backwardEnemySquare_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        --backwardEnemySquare_y;
        --backwardEnemySquare_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, backwardEnemySquare_y, backwardEnemySquare_x ) ){

    // Coords of new piece square.
    i2 = backwardEnemySquare_y;
    j2 = backwardEnemySquare_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalForwardRightLine( Board* board,
                                          const Player* player,
                                          Piece*& piece,
                                          const int32_t i,
                                          const int32_t j,
                                          int32_t& i2,
                                          int32_t& j2,
                                          bool& isKingUnderAttack,
                                          const int32_t LENGTH ){

  int32_t square_x = j;
  int32_t square_y = i;
  int32_t lengthIter = 0;

  if( piece->getPieceColor() == PieceColor::WHITE &&
      i < BoardGlobals::getSize() - 1 &&
      j > 0 ){ // WHITES.

    while( square_y < BoardGlobals::getSize() - 1 &&
           square_x > 0 &&
           lengthIter < LENGTH ){
      ++square_y;
      --square_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[square_y][square_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        --square_y;
        ++square_x;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i > 0 &&
           j < BoardGlobals::getSize() - 1 ){

    while( square_y > 0 &&
           square_x < BoardGlobals::getSize() - 1 &&
           lengthIter < LENGTH ){
      --square_y;
      ++square_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[square_y][square_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        ++square_y;
        --square_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, square_y, square_x ) ){

    // Coords of new piece square.
    i2 = square_y;
    j2 = square_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}



bool Piece::moveDiagonalBackwardLeftLine( Board* board,
                                          const Player* player,
                                          Piece*& piece,
                                          const int32_t i,
                                          const int32_t j,
                                          int32_t& i2,
                                          int32_t& j2,
                                          bool& isKingUnderAttack,
                                          const int32_t LENGTH ){

  int32_t square_x = j;
  int32_t square_y = i;
  int32_t lengthIter = 0;

 if( piece->getPieceColor() == PieceColor::WHITE &&
      i > 0 &&
      j < BoardGlobals::getSize() - 1 ){ // WHITES.

    while( square_y > 0 &&
           square_x < BoardGlobals::getSize() - 1 &&
           lengthIter < LENGTH ){
      --square_y;
      ++square_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[square_y][square_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        ++square_y;
        --square_x;
        break;
      }
    }
  }// BLACKS.
  else if( piece->getPieceColor() == PieceColor::BLACK &&
           i < BoardGlobals::getSize() - 1  &&
           j > 0  ){

    while( square_y < BoardGlobals::getSize() - 1 &&
           square_x > 0 &&
           lengthIter < LENGTH ){
      ++square_y;
      --square_x;
      ++lengthIter;

      Piece* potentialSquare =
      board->changeBoard()[square_y][square_x];
      // If nearest square is busy - go to previous free pos. and break cycle.
      if( potentialSquare ){
        --square_y;
        ++square_x;
        break;
      }
    }
  }

  if ( piece->movePiece( board, player, piece, square_y, square_x ) ){

    // Coords of new piece square.
    i2 = square_y;
    j2 = square_x;

    return true; // Pass next move to opponent.
  }
  else
    return false; // If no coordinates were changed or square is busy - treat next piece.
}

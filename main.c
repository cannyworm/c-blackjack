#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define  DECK_SIZE 52
#define  CARD_VAR_SIZE 13
#define  MAX_CARD 6

static char CARD_SYMBOL[4]       = { 'H', 'T', 'C', 'P'};
static char CARD_VALUE_SYMBOL[4] = { 'A', 'J', 'K', 'Q'};

typedef struct {
    int cards[ DECK_SIZE ];
    int size;
    int pos;
} s_deck;


int card_get_value( int card ) {
    int v = (card % CARD_VAR_SIZE ) + 1;
    return v > 10 ? 10 : v;
}

char card_get_symbol( int card ) {
    return CARD_SYMBOL[ card / CARD_VAR_SIZE ];
}

static char* value_sz[] = {
    "A", "2", "3", "4","5","6","7","8","9","10","J","K","Q"
} ;

const char* card_get_value_sz( int card ) {
    return value_sz[ (card % CARD_VAR_SIZE ) ];
}

void deck_initialize( int deck[DECK_SIZE] ) {

    for ( int i = 0 ; i < DECK_SIZE; i++) {
        deck[i] = i;
    }

}

void deck_shuffle( int deck[DECK_SIZE] ) {

    for ( int i = 0; i < DECK_SIZE * DECK_SIZE; i++) {

        int a1 = i % 52;
        int a2 = rand() % 52;

        int temp = deck[a1];

        deck[a1] = deck[a2];
        deck[a2] = temp;

    }

}

void deck_display( int deck[DECK_SIZE]) {
    for ( int i = 0 ; i < DECK_SIZE; i++ ) {
        int card = deck[i];

        char  symbol = card_get_symbol( card );
        const char *value_sz = card_get_value_sz( card );

        printf("[%c] %s \n", symbol,  value_sz);
    }
}

typedef struct blackjack {

    int deck[DECK_SIZE];
    int deck_pos;

    int player[ MAX_CARD ];
    int player_size;
    int player_stay;

    int dealer[ MAX_CARD ];
    int dealer_size;
    int dealer_stay;

    char winner;

} s_blackjack;

int bjk_give_dealer( s_blackjack * game ) {
    game->dealer[ game->dealer_size++ ] = game->deck[ game->deck_pos++ ];
    return 0;
}

int bjk_give_player( s_blackjack * game ) {
    game->player[ game->player_size++ ] = game->deck[ game->deck_pos++ ];
    return 0;
}

void bjk_initialize( s_blackjack * game ) {

    game->deck_pos = 0;

    memset( &game->player, 0 , sizeof( int ) * MAX_CARD );
    game->player_size = 0;

    memset( &game->dealer, 0 , sizeof( int ) * MAX_CARD );
    game->dealer_size = 0;

    game->winner = 'N';

    deck_initialize( game->deck );
    deck_shuffle( game->deck );

    // deck_display( game->deck );

}



int bjk_get_player_score( s_blackjack * game ) {
    int score = 0;
    for ( int i = 0 ; i < game->player_size; i++) {
        int card_value = card_get_value( game->player[ i ] );
        if ( card_value == 1 && score < 11 )
            score += 11; 
        else
            score += card_value;
    }

    return score;
}

int bjk_get_dealer_score( s_blackjack * game ) {

    int score = 0;
    for ( int i = 0 ; i < game->dealer_size; i++) {
        int card_value = card_get_value( game->dealer[ i ] );
        if ( card_value == 1 && score < 11 )
            score += 11; 
        else
            score += card_value;
    }

    return score;
}


void bjk_start ( s_blackjack * game ) {

    bjk_give_dealer( game );
    bjk_give_dealer( game );

    bjk_give_player( game );
    bjk_give_player( game );

    int pscore = bjk_get_player_score( game );
    int hscore = bjk_get_player_score( game );

    if( pscore == 21 && hscore == 21 ) {
        game->winner = 'D';
        return;
    }

    if ( pscore == 21 ) {
        game->winner = 'P';
    }
    else if ( hscore == 21 ) {
        game->winner = 'H';
    }


}

int bjk_continue( s_blackjack * game , char player_action ) {

    if ( player_action != 'H' && player_action != 'S')
        return -1;

    if ( player_action == 'S') {
        game->player_stay = 1;
    } else {
        bjk_give_player( game );
    }

    int pscore = bjk_get_player_score( game );
    int hscore = bjk_get_dealer_score( game );

    if ( game->player_stay ) {

        game->winner = ( pscore > hscore ) ? 'P' : 'H';

        // DRAW
        if ( pscore == hscore)
            game->winner = 'D';
    }



    return game->winner == 'N';
}


void bjk_display( s_blackjack * game ) {

    printf("DEALER : ");

    for ( int i = 0; i < game->dealer_size; i++ ) {
        int card = game->dealer[ i ];
        if ( i == 0) {
            printf("[?]??");
        }
        else {
            printf(
                "[%c]%s", card_get_symbol( card ), card_get_value_sz( card )
            );
        }
        if ( i < game->dealer_size - 1)
            printf(" ,");
    }

    printf("\n");

    printf("PLAYER : ");

    for ( int i = 0; i < game->player_size; i++ ) {
        int card = game->player[ i ];
        printf(
            "[%c]%s", card_get_symbol( card ), card_get_value_sz( card )
        );
        if ( i < game->player_size - 1)
            printf(" ,");
        else
            printf(" => [%d]", bjk_get_player_score( game ));
    }

    printf("\n");

}




unsigned long urand() {
    FILE * urand_file = fopen("/dev/urandom", "r");

    unsigned long seed = 0;

    fread(
        &seed,
        sizeof( unsigned long),
        1,
        urand_file
    );


    return seed;
}


int main() {

    time_t t;
    time(&t);

    /* Intializes random number generator */
    srand( ( unsigned int )t * ( unsigned int )urand() );


    s_blackjack game = { 0 };  


    bjk_initialize( &game );
    bjk_start( &game );
    bjk_display( &game );

    int bjk_status = game.winner == 'N';
    while (  bjk_status == 1 ) {

        printf("(H)it or (S)tay ? : ");
        char input;
        scanf( "%c", &input);

        while ( bjk_status = bjk_continue( &game , input ) , bjk_status == -1 )
            scanf( "%c", &input);

        bjk_display( &game );
    }

    printf("House  = [%d] \n", bjk_get_dealer_score( &game ));
    printf("Player = [%d] \n", bjk_get_player_score( &game ));

    if ( game.winner == 'P') {
        printf("Player WIN!!! \n");
    } else if ( game.winner == 'H') {
        printf("House WIN!!! \n");
    }
    else {
        printf("Drawn !!! \n");
    }




}

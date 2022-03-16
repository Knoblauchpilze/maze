
# include "Hexagon.hh"

namespace maze {

  HexagonMaze::HexagonMaze(unsigned width, unsigned height):
    Maze(width, height, 6u)
  {}

  void
  HexagonMaze::generate() {
    bool validWall;
    int nbWalls, nbOpen(0), m, n, way(0), bway, cell1, cell2, i, oldId;
    bool *ways = NULL;

    int* id = NULL;

    if(_maze != NULL && _width > 0 && _height > 0)
    {
        /* Allocation du tableau des identifiants et du tableau de direction */
        id = new int[_width * _height];
        ways = new bool[_nbSide];

        if(id == NULL)
        {
            std::cerr << "Unable to allocate memory to store the identifier array" << std::endl;
        }
        else if(ways == NULL)
        {
            std::cerr << "Unable to allocate memory to store the direction array" << std::endl;
        }
        else
        {
            this->Close();

            m = _width;
            n = _height;
            /*nbWalls = (m - 1) * (n - 1);*/
            nbWalls = m * n - 1;

            for(i = 0 ; i < _nbSide ; i++)
            {
                ways[i] = true;
            }

            for(i = 0 ; i < _width * _height ; i++)
            {
                id[i] = i;
            }

            while(nbOpen < nbWalls)
            {
                do
                {
                    /* Remise à zéro des variables */
                    validWall = true;
                    for(i = 0 ; i < _nbSide ; i++)
                    {
                        ways[i] = true;
                    }

                    /* Tirage au sort de la première cellule */
                    cell1 = rand() % (_width * _height);

                    /* Pose des contraintes du mur suivant */
                    /* Cas où les cellules de droite n'existent pas */
                        if(cell1 % _width == _width - 1)
                        {
                            ways[0] = false;
                            ways[5] = false;
                        }
                    /* Cas où les cellules du haut n'existent pas */
                        if(cell1 / _width == 0)
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                ways[1] = false;
                            }
                            else
                            {
                                ways[0] = false;
                                ways[1] = false;
                                ways[2] = false;
                            }
                        }
                    /* Cas où les cellules de gauche n'existent pas */
                        if(cell1 % _width == 0)
                        {
                            ways[2] = false;
                            ways[3] = false;
                        }
                    /* Cas où les cellules du bas n'existent pas */
                        if(cell1 / _width == _height - 1)
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                ways[3] = false;
                                ways[4] = false;
                                ways[5] = false;
                            }
                            else
                            {
                                ways[4] = false;
                            }
                        }

                    /* Tirage au sort de la deuxième cellule */
                    /* way :
                        0 -> right
                        1 -> up
                        2 -> left
                        3 -> down */
                        do
                        {
                            way = rand() % _nbSide;
                        }
                        while(ways[way] == false);
                        if(way == 0)
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                cell2 = cell1 + 1;
                            }
                            else
                            {
                                cell2 = cell1 - _width + 1;
                            }
                        }
                        else if(way == 1)
                        {
                            cell2 = cell1 - _width;
                        }
                        else if(way == 2)
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                cell2 = cell1 - 1;
                            }
                            else
                            {
                                cell2 = cell1 - _width - 1;
                            }
                        }
                        else if(way == 3)
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                cell2 = cell1 + _width - 1;
                            }
                            else
                            {
                                cell2 = cell1 - 1;
                            }
                        }
                        else if(way == 4)
                        {
                            cell2 = cell1 + _width;
                        }
                        else
                        {
                            if((cell1 % _width) % 2 == 0)
                            {
                                cell2 = cell1 + _width + 1;
                            }
                            else
                            {
                                cell2 = cell1 + 1;
                            }
                        }

                    /* On regarde si on peut ouvrir le mur */
                    if(id[cell1] == id[cell2])
                    {
                        /* Pas possible d'ouvrir ce mur */
                        validWall = false;
                    }
                    else
                    {
                        /* Ouverture des deux murs */
                        bway = this->GetOppositeMove(way, this->IsInvertedCell(cell1));
                        _maze[cell1]->OpenDoor(way, true);
                        _maze[cell2]->OpenDoor(bway, true);

                        /* Remplacement de l'identifiant le plus élevé par le plus petit */
                        oldId = id[cell2];
                        for(i = 0 ; i < _width * _height ; i++)
                        {
                            if(id[i] == oldId)
                            {
                                id[i] = id[cell1];
                            }
                        }
                    }

                }
                while(validWall == false);

                nbOpen++;
            }
        }
    }
  }

  unsigned
  HexagonMaze::opposite(unsigned door, bool inverted) const noexcept {
    /// TODO: Handle this.
    return 0u;
  }

  bool
  HexagonMaze::inverted(unsigned door) const noexcept {
    /// TODO: Handle this.
    return 0u;
  }

}

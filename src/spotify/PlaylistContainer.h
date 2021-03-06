#ifndef PLAYLIST_CONTAINER_H
#define PLAYLIST_CONTAINER_H

#include <libspotify/api.h>
#include <vector>
#include "Playlist.h"
#include <v8.h>

class PlaylistContainer {
  public:
    PlaylistContainer(sp_playlistcontainer* _playlistContainer) : playlistContainer(_playlistContainer) {};
    /**
     * Load all playlists into the vector.
     * Needs to be invoked in the spotify thread!
     **/
    void loadPlaylists();
    std::vector<Playlist*> getPlaylists() { return playlists; };

    static Persistent<Function>* getContainerLoadedCallback() { return &containerLoadedCallback; };
    static void setContainerLoadedCallback( Persistent<Function> callback ) { PlaylistContainer::containerLoadedCallback = callback; };
  private:
    sp_playlistcontainer* playlistContainer;
    std::vector<Playlist*> playlists;
    
    static Persistent<Function> containerLoadedCallback;
};

#endif

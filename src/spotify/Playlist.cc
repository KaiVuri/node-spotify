#include "Playlist.h"
#include "../events.h"

std::vector<Track*> Playlist::getTracks() {
  pthread_mutex_lock(&lockingMutex);
  if(!tracksLoaded) {
    auto cb = [&] () { loadTracks(); };
    spotifyService->executeSpotifyAPIcall(cb);
    this->wait();
	}
  pthread_mutex_unlock(&lockingMutex);
	return tracks;
}

void Playlist::setName(std::string _name) {
  this->name = _name;
  call(PLAYLIST_RENAMED);
}

void Playlist::setName(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  Playlist* playlist = node::ObjectWrap::Unwrap<Playlist>(info.Holder());
  pthread_mutex_lock(&playlist->lockingMutex);
  String::Utf8Value v8Str(value);
  playlist->name = *v8Str;
  pthread_mutex_unlock(&playlist->lockingMutex);
}

Handle<Value> Playlist::getName(Local<String> property, const AccessorInfo& info) {
  Playlist* playlist = node::ObjectWrap::Unwrap<Playlist>(info.Holder());
  return String::New(playlist->name.c_str());
}

Handle<Value> Playlist::getId(Local<String> property, const AccessorInfo& info) {
  Playlist* playlist = node::ObjectWrap::Unwrap<Playlist>(info.Holder());
  return Integer::New(playlist->id);
}

Handle<Value> Playlist::getTracks(const Arguments& args) {
  HandleScope scope;
  Playlist* playlist = node::ObjectWrap::Unwrap<Playlist>(args.This());
  
  pthread_mutex_lock(&playlist->lockingMutex);
  if(!playlist->tracksLoaded) {
    auto cb = [=] () { playlist->loadTracks(); };
    spotifyService->executeSpotifyAPIcall(cb);
    playlist->wait();
  }
  pthread_mutex_unlock(&playlist->lockingMutex);
  Local<Array> nTracks = Array::New(playlist->tracks.size());
  for(int i = 0; i < (int)playlist->tracks.size(); i++) {
    nTracks->Set(Number::New(i), playlist->tracks[i]->getV8Object() );
  }
  return scope.Close(nTracks);
}

Handle<Value> Playlist::getImageBase64(const Arguments& args) {
  Playlist* playlist = node::ObjectWrap::Unwrap<Playlist>(args.This());
  if(playlist->playlistImageBase64 != 0) {
    return String::New(playlist->playlistImageBase64);
  }
  return Undefined();
}

void Playlist::init(Handle<Object> target) {
  HandleScope scope;
  Handle<FunctionTemplate> constructorTemplate = NodeWrapped::init("Playlist");

  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), getName, setName);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("id"), getId, emptySetter);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getTracks", getTracks);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getImageBase64", getImageBase64);

  constructor = Persistent<Function>::New(constructorTemplate->GetFunction());
  scope.Close(Undefined());
}

void Playlist::loadTracks() {
  for(int i = 0; i < sp_playlist_num_tracks(playlist); ++i) {
    sp_track* spTrack = sp_playlist_track(playlist, i);
    Track* track = new Track(spTrack);
    tracks.push_back(track);
  }
  tracksLoaded = true;
  done();
}

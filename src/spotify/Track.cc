#include "Track.h"

Handle<Value> Track::getName(Local<String> property, const AccessorInfo& info) {
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  return String::New(track->name.c_str());
}

Handle<Value> Track::getDuration(Local<String> property, const AccessorInfo& info) {
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  return Integer::New(track->duration/1000);
}

Handle<Value> Track::getPopularity(Local<String> property, const AccessorInfo& info) {
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  return Integer::New(track->popularity);
}

Handle<Value> Track::getArtists(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  Local<Array> jsArtists = Array::New(track->artists.size());
  for(int i = 0; i < (int)track->artists.size(); i++) {
    jsArtists->Set(Number::New(i), track->artists[i]->getV8Object() );
  }
  return scope.Close(jsArtists); 
}

Handle<Value> Track::getAlbum(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  return scope.Close(track->album->getV8Object());
}

Handle<Value> Track::getStarred(Local<String> property, const AccessorInfo& info) {
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  return Boolean::New(track->starred);
}

void Track::setStarred(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  Track* track = node::ObjectWrap::Unwrap<Track>(info.Holder());
  track->starred = value->ToBoolean()->Value();
  auto cb = [=] () {
    //This takes an array of pointers to tracks, so we need to tack the adress of the saved spotifyTrack pointer.
    sp_track_set_starred(spotifyService->spotifySession, (sp_track* const*)&track->spotifyTrack, 1, track->starred);
  };
  spotifyService->executeSpotifyAPIcall(cb);
  scope.Close(Undefined());
}

void Track::init(Handle<Object> target) {
  HandleScope scope;
  Handle<FunctionTemplate> constructorTemplate = NodeWrapped::init("Track");
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), getName, emptySetter);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("duration"), getDuration, emptySetter);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("artists"), getArtists, emptySetter);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("album"), getAlbum, emptySetter);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("starred"), getStarred, setStarred);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("popularity"), getPopularity, emptySetter);
  constructor = Persistent<Function>::New(constructorTemplate->GetFunction());
}

type dbReturn = {
  local: Pouchdb.t,
  remote: Most.stream(unit),
};

let init = (dbName: string) : dbReturn => {
  let local = PouchdbImpl.connect(dbName, Config.Database.livePouchDbConfig);
  let remote =
    local
    |> PouchdbImpl.initRemoteSync(dbName, Config.Database.livePouchDbConfig);
  {local, remote};
};
/* let subscribe = (remoteEventCallback: 'a => unit, db) =>
   db |> Most.observe(remoteEventCallback); */
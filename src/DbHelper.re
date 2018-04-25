let init = (dbName: string, remoteEventCallback: 'a => unit) => {
  let db = PouchdbImpl.connect(dbName, Config.Database.livePouchDbConfig);
  db
  |> PouchdbImpl.initRemoteSync(dbName, Config.Database.livePouchDbConfig)
  |> Most.observe(remoteEventCallback)
  |> ignore;
  db;
};
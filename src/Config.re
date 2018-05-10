module App = {
  type t = {
    language: string,
    deviceId: string,
    now: option(Date.t),
  };
  let defaultConfig = {language: "EN", deviceId: "", now: None};
  let toString = config =>
    [|
      config.language,
      config.deviceId,
      switch (config.now) {
      | None => "0"
      | Some(d) => d |> string_of_float
      },
    |]
    |> Js.Array.joinWith("||");
  let getDate = arr =>
    switch (arr |. List.nth(2)) {
    | "0" => None
    | d => Some(d |> float_of_string)
    };
  let fromString = str => {
    let arr = str |> Js.String.split("||") |> Array.to_list;
    if (arr |> List.length == 0) {
      defaultConfig;
    } else {
      {
        language: arr |. List.nth(0),
        deviceId: arr |. List.nth(1),
        now: arr |> List.length === 0 ? None : getDate(arr),
      };
    };
  };
  let appConfigKey = "appConfig";
  let get = () => {
    let maybe = LocalStorage.getValue(appConfigKey);
    switch (maybe) {
    | None => defaultConfig
    | Some(config) => config |> fromString
    };
  };
  let set = (config: t) =>
    LocalStorage.setValue(appConfigKey, config |> toString);
};

module Database = {
  type dbOptions = {
    .
    "auth": {
      .
      "username": string,
      "password": string,
    },
    "auto_compaction": bool,
  };
  type connectionConfig = {
    host: string,
    options: dbOptions,
  };
  type syncOptions = {
    .
    "live": bool,
    "retry": bool,
  };
  type syncConnectionConfig = {
    host: string,
    options: dbOptions,
    syncOptions,
  };
  type pouchDbConfig = {
    local: connectionConfig,
    remote: option(syncConnectionConfig),
  };
  let toSyncConnectionConfig =
      (configDelimited: string)
      : syncConnectionConfig => {
    let syncArr = configDelimited |> Js.String.split("||");
    let config = {
      host: syncArr[0],
      options: {
        "auth": {
          "username": syncArr[1],
          "password": syncArr[2],
        },
        "auto_compaction": true,
      },
      syncOptions: {
        "live": syncArr[3] === "true" ? true : false,
        "retry": syncArr[4] === "true" ? true : false,
      },
    };
    config;
  };
  let toString = (config: syncConnectionConfig) : string =>
    [|
      config.host,
      config.options##auth##username,
      config.options##auth##password,
      config.syncOptions##live ? "true" : "false",
      config.syncOptions##retry ? "true" : "false",
    |]
    |> Js.Array.joinWith("||");
  let syncKey = "sync_config";
  let defaultLocalConnection = {
    host: "",
    options: {
      "auth": {
        "username": "",
        "password": "",
      },
      "auto_compaction": false,
    },
  };
  let livePouchDbConfig = {
    let config = {
      local: defaultLocalConnection,
      remote:
        switch (LocalStorage.getValue(syncKey)) {
        | None => None
        | Some(configDelimited) =>
          Some(configDelimited |> toSyncConnectionConfig)
        },
    };
    config;
  };
};
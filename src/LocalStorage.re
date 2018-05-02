let setValue = (key: string, value: string) =>
  Dom.Storage.localStorage |> Dom.Storage.setItem(key, value);

let getValue = (key: string) =>
  Dom.Storage.localStorage |> Dom.Storage.getItem(key);
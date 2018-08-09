module type Interface = {
  type item;
  type newItem;
  let id: item => string;
  let add: newItem => Js.Promise.t(item);
  let getAll: unit => Js.Promise.t(list(item));
  let update: item => Js.Promise.t(item);
  let remove: (~id: string) => Js.Promise.t(unit);
};
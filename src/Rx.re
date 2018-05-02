let listToStream = promise =>
  promise
  |> Most.fromPromise
  |> Most.map(x => {
       Js.log(x);
       x;
     })
  |> Most.flatMap(webhooks => webhooks |> Most.fromList);
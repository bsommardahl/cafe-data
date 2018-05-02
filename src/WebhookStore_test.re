open Jest;

open Expect;

open Js.Promise;

describe("The Webhook Store", () =>
  describe("When getting all webhooks and returning a stream", () => {
    testAsync("it should return all webhooks as separate events", finish => {
      Js.log("running test");
      Most.(
        Js.Promise.make((~resolve, ~reject) => resolve(. "testing"))
        |> fromPromise
        |> observe(w => Js.log(w))
        |> then_((_) => {
             Js.log("Stream completed");
             finish(expect(1) |> toBe(1));
             Js.Promise.resolve();
           })
      )
      |> ignore;
      ();
    });
    testAsync("most", finish => {
      let add = (x, y) => x + y;
      let isEven = x => x mod 2 === 0 |> Js.Boolean.to_js_boolean;
      Most.(
        fromList([0, 1, 2, 3, 4, 5, 6, 7, 8])
        |> map(add(2))
        |> filter(isEven)
        |> observe(x => Js.log(x))
        |> Js.Promise.then_((_) => {
             Js.log("Stream completed");
             finish(expect(1) |> toBe(1));
             Js.Promise.resolve();
           })
      )
      |> ignore;
      ();
    });
  })
);
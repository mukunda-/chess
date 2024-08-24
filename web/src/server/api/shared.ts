// Zod
import { z } from "zod";

// NextAuth
import type { Session } from "next-auth";

// Ours
import { getAuthenticatedSession } from "@/server/session";

export type APIError = { error: string };
export type APISuccess<T> = { data: T };
export type APIResponse<T> = APIError | APISuccess<T>;

export type ProtectedFunc<I, O> = ({
  session,
  input,
}: {
  session: Session;
  input: I;
}) => Promise<O>;

export function isAPIError(e: unknown): e is APIError {
  return (e as APIError)?.error !== undefined;
}

export function protectedProcedure<I, O>(
  schema: z.ZodType<I>,
  f: ProtectedFunc<I, O>,
): (input: I) => Promise<APIResponse<O>> {
  return async (rawInput: I) => {
    const session = await getAuthenticatedSession();

    if (!session) {
      throw new Error("Unauthenticated access to protected endpoint");
    }

    const parseRes = schema.safeParse(rawInput);
    if (!parseRes.success) {
      return { error: parseRes.error.message };
    }

    const input = parseRes.data;

    try {
      const res = await f({ session, input });
      if (isAPIError(res)) {
        return res;
      }

      return { data: res };
    } catch (e) {
      if (isAPIError(e)) {
        return e;
      }

      throw e;
    }
  };
}

export const noArgs = z.object({});
